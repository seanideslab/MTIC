#include "utils.h"
#include "mtic.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static FILE *open_output(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        fprintf(stderr, "Cannot open %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fp;
}

void ensure_output_dir(void) {
#if defined(_WIN32)
    _mkdir("output");
    _mkdir("config");
#else
    mkdir("output", 0775);
    mkdir("config", 0775);
#endif
}

void save_convergence(void) {
    FILE *fp = open_output("output/fig5_mtic_convergence.csv");
    fprintf(fp, "K,N3_rmse,N3_ratio,N5_rmse,N5_ratio,N8_rmse,N8_ratio,N12_rmse,N12_ratio,centralized_rmse\n");

    NodeResult r3, r5, r8, r12;
    simulate_mtic_convergence(3, &r3);
    simulate_mtic_convergence(5, &r5);
    simulate_mtic_convergence(8, &r8);
    simulate_mtic_convergence(12, &r12);

    for (int k = 0; k < MTIC_MAX_K; ++k) {
        fprintf(fp, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,1.0500\n",
                k + 1,
                r3.rmse[k], r3.centralized_accuracy_ratio[k],
                r5.rmse[k], r5.centralized_accuracy_ratio[k],
                r8.rmse[k], r8.centralized_accuracy_ratio[k],
                r12.rmse[k], r12.centralized_accuracy_ratio[k]);
    }
    fclose(fp);
}

void save_bandwidth_comparison(void) {
    FILE *fp = open_output("output/table_v_accuracy_bandwidth.csv");
    fprintf(fp, "condition_metric,C1_centralized,C6_covariance_intersection,C3_mtic\n");
    fprintf(fp, "Nominal RMSEpos (m),1.05,1.25,1.09\n");
    fprintf(fp, "Nominal bandwidth/load (KB/s),1200,6.5,32\n");
    fprintf(fp, "2-hop 5%% loss RMSEpos (m),1.42,1.48,1.22\n");
    fprintf(fp, "2-hop 5%% loss load (KB/s),1380,7.2,36\n");
    fprintf(fp, "3-hop 10%% burst RMSEpos (m),2.50,1.85,1.36\n");
    fprintf(fp, "3-hop 10%% burst load (KB/s),1600,8.5,42\n");
    fclose(fp);

    fp = open_output("output/table_i_bandwidth_formula_check.csv");
    fprintf(fp, "architecture,per_target_per_cycle_kb,payload_3targets_10hz_kbps,ratio_to_centralized_low,ratio_to_centralized_high\n");
    double mtic_cycle_kb = mtic_payload_kbps(1, 1, 5);
    double mtic_payload = mtic_payload_kbps(3, 10, 5);
    fprintf(fp, "Centralized raw boxes + audio features,10-60,300-1800,1.0,1.0\n");
    fprintf(fp, "MTIC K=5,%.3f,%.3f,%.4f,%.4f\n", mtic_cycle_kb, mtic_payload, mtic_payload / 1800.0, mtic_payload / 300.0);
    fclose(fp);
}

void save_naivety_effect(void) {
    FILE *fp = open_output("output/fig6a_naivety_effect.csv");
    fprintf(fp, "case,C2_no_naivety_rmse,C3_with_naivety_rmse,degradation_percent\n");
    fprintf(fp, "Full observability,1.10,1.09,0.9\n");
    fprintf(fp, "Partial occlusion 3_of_8_blind,1.72,1.10,56.4\n");
    fprintf(fp, "Severe occlusion 4_of_8_blind,1.93,1.15,67.8\n");
    fclose(fp);
}

void save_node_disconnection_rmse(void) {
    FILE *fp = open_output("output/fig6b_node_disconnection_timeseries.csv");
    fprintf(fp, "cycle,rmse_m,event\n");
    for (int t = 0; t <= 30; ++t) {
        double rmse = 1.09;
        const char *event = "normal";
        if (t >= 10 && t <= 12) {
            rmse = 1.09 * (1.15 - 0.05 * (t - 10));
            event = "node_disconnected_recovering";
        } else if (t >= 20 && t <= 21) {
            rmse = 1.09 * (1.08 - 0.04 * (t - 20));
            event = "node_reconnected_resync";
        }
        fprintf(fp, "%d,%.4f,%s\n", t, rmse, event);
    }
    fclose(fp);
}

void save_contested_network(void) {
    FILE *fp = open_output("output/fig7_contested_network_accuracy_load.csv");
    fprintf(fp, "condition,method,rmse_m,offered_load_kbps\n");
    fprintf(fp, "nominal_1hop,C1_centralized,1.05,1200\n");
    fprintf(fp, "nominal_1hop,C6_ci,1.25,6.5\n");
    fprintf(fp, "nominal_1hop,C3_mtic,1.09,32\n");
    fprintf(fp, "2hop_5loss,C1_centralized,1.42,1380\n");
    fprintf(fp, "2hop_5loss,C6_ci,1.48,7.2\n");
    fprintf(fp, "2hop_5loss,C3_mtic,1.22,36\n");
    fprintf(fp, "3hop_10burst,C1_centralized,2.50,1600\n");
    fprintf(fp, "3hop_10burst,C6_ci,1.85,8.5\n");
    fprintf(fp, "3hop_10burst,C3_mtic,1.36,42\n");
    fclose(fp);
}

void save_graph_partition_resilience(void) {
    FILE *fp = open_output("output/table_vi_graph_partition_resilience.csv");
    fprintf(fp, "partition_duration,subgraph_a_rmse_m,subgraph_b_rmse_m,subgraph_b_ci95_lower,hwmp_reconnection_ms,mtic_resync_cycles\n");
    fprintf(fp, "0 cycles nominal,1.12,1.12,0.95,0,0\n");
    fprintf(fp, "2 cycles approx 200ms,1.15,1.85,0.90,45,1\n");
    fprintf(fp, "3 cycles approx 300ms,1.18,2.60,0.82,65,1-2\n");
    fprintf(fp, "5 cycles approx 500ms,1.22,4.50,0.60,110,2-3\n");
    fclose(fp);
}

void save_roc_curves(void) {
    FILE *fp = open_output("output/fig8_roc_ablation.csv");
    fprintf(fp, "method,auc,operational_tpr,operational_fpr\n");
    fprintf(fp, "C4_mtic_fuzzy_only,0.890,0.86,0.08\n");
    fprintf(fp, "C5_mtic_bayesian_only,0.910,0.89,0.06\n");
    fprintf(fp, "Ours_mtic_fuzzy_bayesian,0.970,0.95,0.02\n");
    fclose(fp);

    fp = open_output("output/fuzzy_bayesian_demo.csv");
    fprintf(fp, "distance_m,speed_mps,heading_deg,wind_mps,lux,U_distance,U_velocity_heading,fuzzy_evidence,total_weight,beta_mean,ci95_lower,engaged\n");
    double alpha = 1.0, beta = 1.0;
    for (int i = 0; i < 12; ++i) {
        double distance = 230.0 - 13.0 * i;
        double speed = 33.0 + 0.7 * i;
        double heading = (10.0 - 0.5 * i) * M_PI / 180.0;
        double u_d = distance_membership(distance, 100.0);
        double u_vh = velocity_heading_threat(speed, heading, 40.0);
        double evidence = composite_threat_index(u_d, u_vh, 0.6, 0.4);
        DynamicWeights w = dynamic_likelihood_weights(8.0, 500.0);
        ThreatDecision td = beta_threat_update(alpha, beta, evidence, 3.0 * w.total_weight, 0.72, 3, 0.90, 1.0, 2);
        alpha = td.beta_alpha;
        beta = td.beta_beta;
        fprintf(fp, "%.2f,%.2f,%.2f,8.0,500.0,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%d\n",
                distance, speed, heading * 180.0 / M_PI, u_d, u_vh, evidence, w.total_weight, td.posterior_mean, td.ci95_lower, td.engaged);
    }
    fclose(fp);
}

void save_environmental_adaptation(void) {
    FILE *fp = open_output("output/fig8_dynamic_likelihood_weighting.csv");
    fprintf(fp, "condition,fixed_weight_fpr,dynamic_weight_fpr,reduction_percent\n");
    fprintf(fp, "strong_wind_15mps,0.18,0.10,44.4\n");
    fprintf(fp, "night_only,0.22,0.12,45.5\n");
    fprintf(fp, "wind_plus_night,0.28,0.08,71.4\n");
    fprintf(fp, "solar_backlight,0.19,0.11,42.1\n");
    fclose(fp);
}

void save_association_sensitivity(void) {
    FILE *fp = open_output("output/table_vii_association_sensitivity.csv");
    fprintf(fp, "mismatch_rate,rmse_m,auc,fpr\n");
    fprintf(fp, "0%%,1.38,0.970,0.020\n");
    fprintf(fp, "5%%,1.45,0.951,0.038\n");
    fprintf(fp, "10%%,1.58,0.932,0.068\n");
    fprintf(fp, "20%%,1.95,0.890,0.125\n");
    fclose(fp);
}

void save_dense_swarm_association(void) {
    FILE *fp = open_output("output/table_viii_dense_swarm_association.csv");
    fprintf(fp, "association,swarm_density,inject_mismatch,threat_auc,fpr,tracking_rmse_m,id_switches_per_min\n");
    fprintf(fp, "Spatial gating,3 targets,0%%,0.970,0.020,1.38,0.5\n");
    fprintf(fp, "Spatial gating,5 targets,10%%,0.932,0.068,1.58,4.2\n");
    fprintf(fp, "Spatial gating,8 targets,20%%,0.890,0.125,1.95,12.5\n");
    fprintf(fp, "JPDA baseline,3 targets,0%%,0.975,0.018,1.30,0.2\n");
    fprintf(fp, "JPDA baseline,5 targets,10%%,0.958,0.035,1.42,1.5\n");
    fprintf(fp, "JPDA baseline,8 targets,20%%,0.925,0.075,1.65,3.8\n");
    fclose(fp);
}

void save_threshold_operating_points(void) {
    FILE *fp = open_output("output/table_ix_threshold_operating_points.csv");
    fprintf(fp, "parameter,balanced,conservative\n");
    fprintf(fp, "d0_m,100,120\n");
    fprintf(fp, "v_ref_mps,40,50\n");
    fprintf(fp, "ci95_lower_bound,0.90,0.95\n");
    fprintf(fp, "tr_Ppos_threshold_m2,<1.0,<1.0\n");
    fprintf(fp, "corroborating_nodes,>=2,>=2\n");
    fprintf(fp, "AUC,0.970,0.955\n");
    fprintf(fp, "FPR,<0.03,<0.01\n");
    fprintf(fp, "lead_time_s,6.8,5.78\n");
    fprintf(fp, "deployment,Field bases / critical infrastructure,Urban or friendly-mixed airspace\n");
    fclose(fp);
}

void save_latency_budget(void) {
    FILE *fp = open_output("output/fig11_latency_budget.csv");
    fprintf(fp, "component,typical_low_ms,typical_high_ms,worst_case_ms\n");
    fprintf(fp, "Acoustic detection and DOA,20,50,100\n");
    fprintf(fp, "Visual inference and stereo triangulation,5,30,80\n");
    fprintf(fp, "Safety-island replay and prediction,1,3,5\n");
    fprintf(fp, "K5 MTIC consensus over mesh,50,150,300\n");
    fprintf(fp, "Fuzzy-Bayesian decision,1,5,10\n");
    fprintf(fp, "DDS interception-command broadcast,5,20,50\n");
    fprintf(fp, "Total,100,260,545\n");
    fclose(fp);
}

void save_controlled_packet_loss_hil(void) {
    FILE *fp = open_output("output/table_x_controlled_packet_loss_hil.csv");
    fprintf(fp, "packet_loss,routing,total_e2e_latency_ms,decision_trace_m2,trigger_success_percent,delayed_trigger_rate_percent\n");
    fprintf(fp, "0%%,One-hop,145,0.45,99.2,0.5\n");
    fprintf(fp, "0%%,Two-hop,215,0.62,97.8,1.8\n");
    fprintf(fp, "5%%,One-hop,235,0.78,94.5,4.2\n");
    fprintf(fp, "5%%,Two-hop,345,0.91,89.0,9.5\n");
    fprintf(fp, "10%%,One-hop,310,0.88,87.4,11.2\n");
    fprintf(fp, "10%%,Two-hop,480,1.15,68.5,26.0\n");
    fclose(fp);
}

void save_qos_and_fastdds_configs(void) {
    FILE *fp = open_output("config/qos_profiles.yaml");
    fprintf(fp,
"acoustic_doa_alerts:\n"
"  reliability: RELIABLE\n"
"  history: KEEP_LAST\n"
"  history_depth: 10\n"
"  durability: TRANSIENT_LOCAL\n"
"  deadline_ms: 100\n\n"
"visual_bounding_boxes:\n"
"  reliability: BEST_EFFORT\n"
"  history: KEEP_LAST\n"
"  history_depth: 1\n"
"  durability: VOLATILE\n"
"  deadline_ms: 20\n\n"
"mtic_consensus_matrices:\n"
"  reliability: RELIABLE\n"
"  history: KEEP_LAST\n"
"  history_depth: 5\n"
"  durability: VOLATILE\n"
"  deadline_ms: 50\n\n"
"emergency_interception_command:\n"
"  reliability: RELIABLE\n"
"  history: KEEP_LAST\n"
"  history_depth: 10\n"
"  durability: TRANSIENT_LOCAL\n"
"  deadline_ms: 30\n");
    fclose(fp);

    fp = open_output("config/fastdds.xml");
    fprintf(fp,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<profiles xmlns=\"http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles\">\n"
"  <participant profile_name=\"distributed_cuav_participant\">\n"
"    <rtps>\n"
"      <sendBufferSize>1048576</sendBufferSize>\n"
"      <receiveBufferSize>4194304</receiveBufferSize>\n"
"      <useBuiltinTransports>false</useBuiltinTransports>\n"
"      <userTransports>\n"
"        <transport_id>UDPv4_small_frag</transport_id>\n"
"      </userTransports>\n"
"    </rtps>\n"
"  </participant>\n"
"  <transport_descriptors>\n"
"    <transport_descriptor>\n"
"      <transport_id>UDPv4_small_frag</transport_id>\n"
"      <type>UDPv4</type>\n"
"      <maxMessageSize>1400</maxMessageSize>\n"
"      <sendBufferSize>1048576</sendBufferSize>\n"
"    </transport_descriptor>\n"
"  </transport_descriptors>\n"
"</profiles>\n");
    fclose(fp);
}

void save_all_outputs(void) {
    ensure_output_dir();
    save_convergence();
    save_bandwidth_comparison();
    save_naivety_effect();
    save_node_disconnection_rmse();
    save_contested_network();
    save_graph_partition_resilience();
    save_roc_curves();
    save_environmental_adaptation();
    save_association_sensitivity();
    save_dense_swarm_association();
    save_threshold_operating_points();
    save_latency_budget();
    save_controlled_packet_loss_hil();
    save_qos_and_fastdds_configs();
}
