#ifndef MTIC_H
#define MTIC_H

#include <stddef.h>

#define MTIC_STATE_DIM 9
#define MTIC_MAX_K 20
#define MTIC_MAX_NODES 12

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double rmse[MTIC_MAX_K];
    double centralized_accuracy_ratio[MTIC_MAX_K];
} NodeResult;

typedef struct {
    double x[MTIC_STATE_DIM];
    double covariance_diag[MTIC_STATE_DIM];
    double imm_mode_prob[3];
    double prediction_03s[MTIC_STATE_DIM];
    double timestamp_sec;
    int observed;
} StatePacket;

typedef struct {
    double information_diag[MTIC_STATE_DIM];
    double information_state[MTIC_STATE_DIM];
    int observed;
} InformationPacket;

typedef struct {
    double beta_alpha;
    double beta_beta;
    double posterior_mean;
    double ci95_lower;
    int engaged;
} ThreatDecision;

typedef struct {
    double acoustic_weight;
    double visual_weight;
    double total_weight;
} DynamicWeights;

void simulate_mtic_convergence(int nodes, NodeResult *result);
double mtic_payload_kbps(int targets, int update_hz, int consensus_rounds);
double mtic_offered_kbps(int targets, int update_hz, int consensus_rounds, double protocol_overhead, double retransmission_rate);

InformationPacket state_to_information(const StatePacket *packet);
InformationPacket mtic_zero_observation_gain(void);
void mtic_force_positive_floor(InformationPacket *packet, double min_eigenvalue_floor);

/* Fuzzy-Bayesian threat assessment from the revised paper. */
double distance_membership(double distance_m, double d0_m);
double velocity_heading_threat(double speed_mps, double heading_rad, double v_ref_mps);
double composite_threat_index(double distance_u, double velocity_heading_u, double w_distance, double w_velocity_heading);
DynamicWeights dynamic_likelihood_weights(double wind_mps, double lux);
ThreatDecision beta_threat_update(double prior_alpha, double prior_beta, double fuzzy_evidence, double evidence_weight,
                                  double covariance_trace_m2, int corroborating_nodes,
                                  double ci_lower_threshold, double covariance_trace_threshold, int min_nodes);

#ifdef __cplusplus
}
#endif

#endif
