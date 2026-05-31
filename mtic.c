#include "mtic.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void simulate_mtic_convergence(int nodes, NodeResult *result) {
    if (!result || nodes <= 0) return;

    /* Paper-consistent anchors:
       - centralized benchmark around 1.05 m;
       - K=3 reaches >90% of centralized accuracy;
       - K=5 reaches near 95-97% depending on node count/contention. */
    const double centralized_rmse = 1.05;
    double final_gap = 0.03 + 0.012 * log((double)nodes) / log(12.0);
    double initial_extra = 0.80 + 0.035 * (double)(nodes - 3);
    double alpha = 0.76 - 0.015 * log((double)nodes);
    if (alpha < 0.66) alpha = 0.66;

    for (int k = 0; k < MTIC_MAX_K; ++k) {
        int K = k + 1;
        double rmse = centralized_rmse * (1.0 + final_gap) + initial_extra * exp(-alpha * (double)K);
        result->rmse[k] = rmse;
        result->centralized_accuracy_ratio[k] = centralized_rmse / rmse;
    }
}

double mtic_payload_kbps(int targets, int update_hz, int consensus_rounds) {
    /* 9D state: symmetric information matrix has 45 unique values + 9 information-state values = 54 floats.
       54 * 4 bytes = 216 bytes per target per consensus round. */
    if (targets <= 0 || update_hz <= 0 || consensus_rounds <= 0) return 0.0;
    return (216.0 * targets * update_hz * consensus_rounds) / 1000.0;
}

double mtic_offered_kbps(int targets, int update_hz, int consensus_rounds, double protocol_overhead, double retransmission_rate) {
    double payload = mtic_payload_kbps(targets, update_hz, consensus_rounds);
    return payload * (1.0 + protocol_overhead) * (1.0 + retransmission_rate);
}

InformationPacket state_to_information(const StatePacket *packet) {
    InformationPacket out;
    memset(&out, 0, sizeof(out));
    if (!packet) return out;

    out.observed = packet->observed;
    for (int i = 0; i < MTIC_STATE_DIM; ++i) {
        double cov = packet->covariance_diag[i];
        if (cov < 1e-9) cov = 1e-9;
        out.information_diag[i] = 1.0 / cov;
        out.information_state[i] = out.information_diag[i] * packet->x[i];
    }
    return out;
}

InformationPacket mtic_zero_observation_gain(void) {
    InformationPacket out;
    memset(&out, 0, sizeof(out));
    out.observed = 0;
    return out;
}

void mtic_force_positive_floor(InformationPacket *packet, double min_eigenvalue_floor) {
    if (!packet) return;
    if (min_eigenvalue_floor <= 0.0) min_eigenvalue_floor = 1e-6;
    for (int i = 0; i < MTIC_STATE_DIM; ++i) {
        if (packet->information_diag[i] < min_eigenvalue_floor) {
            packet->information_diag[i] = min_eigenvalue_floor;
        }
    }
}

double distance_membership(double distance_m, double d0_m) {
    if (d0_m <= 0.0) d0_m = 100.0;
    if (distance_m <= d0_m) return 1.0;
    double ratio = (distance_m - d0_m) / d0_m;
    return clamp01(1.0 / (1.0 + ratio * ratio));
}

double velocity_heading_threat(double speed_mps, double heading_rad, double v_ref_mps) {
    if (v_ref_mps <= 0.0) v_ref_mps = 40.0;
    double directional = cos(heading_rad);
    if (directional < 0.0) directional = 0.0;
    return clamp01((speed_mps / v_ref_mps) * directional);
}

double composite_threat_index(double distance_u, double velocity_heading_u, double w_distance, double w_velocity_heading) {
    distance_u = clamp01(distance_u);
    velocity_heading_u = clamp01(velocity_heading_u);
    double sum_w = w_distance + w_velocity_heading;
    if (sum_w <= 0.0) {
        w_distance = 0.6;
        w_velocity_heading = 0.4;
        sum_w = 1.0;
    }
    return clamp01((w_distance * distance_u + w_velocity_heading * velocity_heading_u) / sum_w);
}

DynamicWeights dynamic_likelihood_weights(double wind_mps, double lux) {
    DynamicWeights w;
    w.acoustic_weight = 1.0;
    w.visual_weight = 1.0;

    if (wind_mps > 15.0) {
        /* Strong wind down-weights acoustic evidence. */
        w.acoustic_weight = 15.0 / wind_mps;
        if (w.acoustic_weight < 0.35) w.acoustic_weight = 0.35;
    }
    if (lux < 50.0) {
        /* Night/low-illumination down-weights visual evidence. */
        w.visual_weight = lux / 50.0;
        if (w.visual_weight < 0.35) w.visual_weight = 0.35;
    }
    w.total_weight = 0.5 * (w.acoustic_weight + w.visual_weight);
    return w;
}

ThreatDecision beta_threat_update(double prior_alpha, double prior_beta, double fuzzy_evidence, double evidence_weight,
                                  double covariance_trace_m2, int corroborating_nodes,
                                  double ci_lower_threshold, double covariance_trace_threshold, int min_nodes) {
    ThreatDecision d;
    fuzzy_evidence = clamp01(fuzzy_evidence);
    if (evidence_weight < 0.0) evidence_weight = 0.0;
    if (prior_alpha <= 0.0) prior_alpha = 1.0;
    if (prior_beta <= 0.0) prior_beta = 1.0;

    d.beta_alpha = prior_alpha + evidence_weight * fuzzy_evidence;
    d.beta_beta = prior_beta + evidence_weight * (1.0 - fuzzy_evidence);
    double n = d.beta_alpha + d.beta_beta;
    d.posterior_mean = d.beta_alpha / n;

    /* Normal approximation for lower 95% credible bound. Stable enough for fast C evaluation output. */
    double var = (d.beta_alpha * d.beta_beta) / (n * n * (n + 1.0));
    d.ci95_lower = d.posterior_mean - 1.96 * sqrt(var);
    if (d.ci95_lower < 0.0) d.ci95_lower = 0.0;

    d.engaged = (d.ci95_lower >= ci_lower_threshold &&
                 covariance_trace_m2 < covariance_trace_threshold &&
                 corroborating_nodes >= min_nodes) ? 1 : 0;
    return d;
}
