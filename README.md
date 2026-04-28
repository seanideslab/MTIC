# Distributed Counter-UAV Early Warning: Acoustic-Visual Information Consensus and Fuzzy-Bayesian Threat Assessment

This repository provides a lightweight C-based reproduction package for the **Paper B** fusion, threat-assessment, communication, and deployment-oriented evaluation layer of the manuscript:

**“Distributed Counter-UAV Early Warning: Acoustic-Visual Information Consensus and Fuzzy-Bayesian Threat Assessment”**

The current manuscript and supplementary package are prepared for submission to **Defence Technology**. The repository is intended to generate reproducible CSV artifacts, preserve deployment-related configuration examples, and document the low-level hardware assumptions used in the paper.

\---

## 1\. Repository Purpose

This package has two complementary goals:

1. **Result reproduction for Paper B**  
Generate CSV outputs corresponding to the manuscript’s MTIC, fuzzy–Bayesian, contested-network, threshold, latency, and hardware-in-the-loop evaluation results.
2. **Deployment-context documentation**  
Preserve the practical hardware and middleware notes used in the study, including:

   * Jetson Orin Nano platform configuration
   * INMP441 I2S microphone integration
   * Preempt-RT scheduling and CPU isolation
   * software-assisted IEEE 1588 PTP with GPS PPS
   * ROS 2 QoS profiles
   * Fast DDS wireless-mesh transport tuning

\---

## 2\. Scope of This Repository

This repository is a **simulation/result-generation and documentation package**. It is designed to reproduce the **fusion / decision / evaluation layer** of Paper B at the CSV and configuration-file level.

It does **not** replace:

* ROS 2 runtime nodes
* Jetson device drivers
* DDS middleware binaries
* the Part-I perception stack
* real camera or microphone acquisition code
* YOLO / TensorRT weights
* full safety-island onboard implementation

In other words, this code models the **Paper B layer** of the system in a reproducible offline form rather than as a complete deployed edge-defense stack.

\---

## 3\. Relationship to Part I

Paper B assumes that the upstream **Part I** system provides replay-consistent local state packets, including:

* 3D position / velocity / acceleration estimate
* covariance
* IMM mode probabilities
* short-horizon prediction
* synchronized timestamp
* observation / naïvety flag

This repository therefore focuses on:

* distributed MTIC fusion
* fuzzy–Bayesian threat assessment
* communication and latency evaluation
* deployment-oriented table / figure reproduction

It does not implement the full Part-I acoustic–visual perception and local-state generation pipeline.

\---

## 4\. What Changed from the Original Uploaded Code

The originally uploaded code was much smaller and only reproduced a limited subset of the Paper B results. The current repository has been expanded to cover the revised submission’s major evaluations, including:

* MTIC convergence for `N = 3, 5, 8, 12` nodes
* Table I bandwidth formula verification for 9D information-state packets
* Table V centralized / CI / MTIC accuracy-bandwidth comparison
* naïvety-handling and blind-node robustness
* node disconnection and recovery time-series
* contested-network accuracy-load trade-off
* Table VI graph-partition resilience
* fuzzy–Bayesian helper outputs
* ROC ablation output for full / Bayesian-only / fuzzy-only variants
* dynamic likelihood weighting under degraded sensing conditions
* Table VII association-error sensitivity
* Table VIII dense-swarm spatial-gating versus JPDA baseline
* Table IX balanced and conservative operating points
* Fig. 11 end-to-end latency budget
* Table X controlled packet-loss HIL closure
* ROS 2 QoS YAML and Fast DDS XML configuration examples

\---

## 5\. Current Repository Layout

```text
.
├── Makefile
├── README.md
├── main.c
├── main.o
├── mtic.c
├── mtic.h
├── utils.c
├── utils.h
├── cuav\_mtic\_sim
├── config/
│   ├── fastdds.xml
│   └── qos\_profiles.yaml
└── output/
    ├── fig5\_mtic\_convergence.csv
    ├── fig6a\_naivety\_effect.csv
    ├── fig6b\_node\_disconnection\_timeseries.csv
    ├── fig7\_contested\_network\_accuracy\_load.csv
    ├── fig8\_dynamic\_likelihood\_weighting.csv
    ├── fig8\_roc\_ablation.csv
    ├── fig11\_latency\_budget.csv
    ├── fuzzy\_bayesian\_demo.csv
    ├── table\_i\_bandwidth\_formula\_check.csv
    ├── table\_ix\_threshold\_operating\_points.csv
    ├── table\_v\_accuracy\_bandwidth.csv
    ├── table\_vi\_graph\_partition\_resilience.csv
    ├── table\_vii\_association\_sensitivity.csv
    ├── table\_viii\_dense\_swarm\_association.csv
    └── table\_x\_controlled\_packet\_loss\_hil.csv
```

### Notes

* `cuav\_mtic\_sim` and `main.o` are **build artifacts** generated locally after compilation.
* The `output/` directory contains both **main-manuscript outputs** and a small number of **supplementary/helper outputs**.

\---

## 6\. Build and Run

### 6.1 Requirements

* Linux / WSL / macOS terminal
* `gcc`
* `make`
* standard C math library `libm`

### 6.2 Build

```bash
make
```

### 6.3 Run

```bash
./cuav\_mtic\_sim
```

or

```bash
make run
```

### 6.4 Clean

```bash
make clean
```

\---

## 7\. Output Mapping

Generated files are written to:

```text
output/
config/
```

### 7.1 Main-manuscript outputs

|File|Manuscript mapping|Purpose|
|-|-|-|
|`output/fig5\_mtic\_convergence.csv`|Fig. 5|MTIC convergence vs. consensus rounds|
|`output/table\_i\_bandwidth\_formula\_check.csv`|Table I|9D information-state packet bandwidth verification|
|`output/table\_v\_accuracy\_bandwidth.csv`|Table V|Centralized / CI / MTIC accuracy-bandwidth comparison|
|`output/fig6a\_naivety\_effect.csv`|Fig. 6(a)|Blind-node / naïvety-handling robustness|
|`output/fig6b\_node\_disconnection\_timeseries.csv`|Fig. 6(b)|Node disconnection and recovery time-series|
|`output/fig7\_contested\_network\_accuracy\_load.csv`|Fig. 7(b) and related text|Contested-link accuracy-load trade-off|
|`output/table\_vi\_graph\_partition\_resilience.csv`|Table VI|Temporary graph-partition resilience|
|`output/fig8\_dynamic\_likelihood\_weighting.csv`|Fig. 8|Dynamic likelihood weighting under degraded sensing|
|`output/table\_vii\_association\_sensitivity.csv`|Table VII|Association-error sensitivity|
|`output/table\_viii\_dense\_swarm\_association.csv`|Table VIII|Spatial-gating vs. JPDA dense-swarm comparison|
|`output/table\_ix\_threshold\_operating\_points.csv`|Table IX|Balanced and conservative decision-threshold settings|
|`output/fig11\_latency\_budget.csv`|Fig. 11|End-to-end latency budget|
|`output/table\_x\_controlled\_packet\_loss\_hil.csv`|Table X|Controlled packet-loss HIL closure|

### 7.2 Supplementary/helper outputs

|File|Supplementary/helper mapping|Purpose|
|-|-|-|
|`output/fig8\_roc\_ablation.csv`|Supplementary Fig. S2(a) support|ROC ablation of full / Bayesian-only / fuzzy-only threat assessment|
|`output/fuzzy\_bayesian\_demo.csv`|auxiliary helper output|Demonstration sequence of threat evidence accumulation|

### 7.3 Configuration files

|File|Purpose|
|-|-|
|`config/qos\_profiles.yaml`|ROS 2 QoS profile examples for heterogeneous C-UAV data flows|
|`config/fastdds.xml`|Fast DDS small-fragment wireless-mesh transport profile|

\---

## 8\. Paper-B Algorithm Coverage

### 8.1 MTIC layer

The code reflects the manuscript’s Multi-Target Information Consensus logic at evaluation-output level, including:

* information-state packet sizing for a 9D state
* `K`-round consensus behavior
* centralized, covariance-intersection, and MTIC comparison
* blind-node zero-information contribution
* node failure and recovery behavior
* graph-partition resilience

### 8.2 Fuzzy–Bayesian threat assessment

The implemented helper logic and CSV outputs cover:

* distance membership function
* velocity-heading joint threat function
* composite fuzzy threat index
* Bayesian evidence accumulation
* credible-interval based engagement trigger
* dynamic likelihood weighting for wind / low-light / representative degraded sensing
* decision-threshold operating points

### 8.3 Communication and HIL evaluation

The generated outputs include:

* contested-network RMSE/load comparison
* packet-loss and burst-loss cases
* controlled one-hop / two-hop packet-loss HIL closure
* latency budget decomposition for:

  * acoustic detection
  * visual inference
  * safety-island replay
  * MTIC consensus
  * fuzzy–Bayesian decision
  * DDS emergency broadcast

\---

## 9\. Platform Overview

|Component|Specification|
|-|-|
|Edge compute|NVIDIA Jetson Orin Nano 8 GB, JetPack 6.0 / L4T R36.3|
|OS kernel|Ubuntu 22.04 + Preempt-RT patch|
|Neural inference|Ampere GPU, Tensor Cores, YOLO11n via TensorRT|
|CPU cluster|ARM Cortex-A78AE hex-core|
|Wireless mesh|802.11s ad-hoc mesh|
|Time sync|Software IEEE 1588 PTP + GPS PPS|

\---

## 10\. I2S MEMS Microphone Integration

Each node uses **INMP441** omnidirectional MEMS microphones:

* SNR: 61 dB
* Frequency response: 60 Hz – 15 kHz
* Interface: I2S on the Jetson Orin Nano 40-pin header

### 10.1 Electrical connections

|INMP441 Pin|Jetson Header Pin|Signal|Notes|
|-|-|-|-|
|VDD|Pin 1|3.3 V DC|Shared power|
|GND|Pin 6|Ground|Common reference|
|SD|Pin 38|I2S2\_SDIN|Digital audio input|
|SCK|Pin 12|I2S2\_SCLK|Serial bit clock|
|WS|Pin 35|I2S2\_FS|Word-select / frame-sync|
|L/R|Pin 9 / GND or 3.3 V|Channel select|Low = left, high = right|

\---

## 11\. Device-Tree Configuration for JetPack 6 / L4T R36.3

On JetPack 6, the Jetson Orin Nano 40-pin header defaults I2S pins to GPIO mode. Since U-Boot no longer supports dynamic `extlinux.conf FDTOVERLAYS` in the older workflow, the repository preserves a **physical DTB merge** approach.

### 11.1 Example DTB merge procedure

```bash
RUNTIME\_DTB=/boot/dtb/kernel\_tegra234-p3767-0004-p3768-0000-a0.dtb

sudo fdtoverlay -i ${RUNTIME\_DTB}     -o /boot/dtb/merged\_audio.dtb     /boot/tegra234-p3767-0000-p3768-0000-audio.dtbo
```

### 11.2 Verification

```bash
aplay -l
dmesg | grep i2s
```

\---

## 12\. ALSA Capture Configuration

After successful DTB merge, configure ALSA for multi-channel capture.

### 12.1 Example `/etc/asound.conf`

```bash
pcm.i2s\_mic\_array {
    type hw
    card tegra-i2s-apbif
    device 0
}

pcm.mic\_array\_48k {
    type plug
    slave {
        pcm i2s\_mic\_array
        rate 48000
        channels 4
        format S32\_LE
    }
}
```

### 12.2 Capture parameters used in the study

|Parameter|Value|
|-|-|
|Sampling rate|48,000 Hz|
|Bit depth|32-bit little-endian (`S32\_LE`)|
|Channels|4|
|CPU overhead|1–2%|
|Buffer size|1024 frames|

\---

## 13\. CPU Isolation and Real-Time Scheduling

### 13.1 Example boot parameters

```text
APPEND ... isolcpus=4,5 nohz\_full=4,5 rcu\_nocbs=4,5 irqaffinity=0-3
```

### 13.2 Example isolation commands

```bash
sudo cset shield --cpu=4,5 --kthread=on
sudo chrt -f 95 taskset -c 4 python3 i2s\_capture\_node.py
```

### 13.3 Example thread priorities

|Task|Scheduling|Priority|Core|
|-|-|-:|-|
|Safety-island kernel tick|SCHED\_FIFO|95|4|
|I2S capture handler|SCHED\_FIFO|94|4|
|Camera frame capture|SCHED\_FIFO|93|5|
|MTIC consensus module|SCHED\_OTHER|—|0–3|
|YOLO / TensorRT inference|SCHED\_OTHER|—|GPU|

\---

## 14\. PTP Time Synchronization Setup

### 14.1 Software PTP with GPS PPS

```bash
sudo apt install linuxptp

# Example run
sudo ptp4l -i eth0 -m
sudo phc2sys -s eth0 -w -m -u 1
```

### 14.2 Measured synchronization performance

|Link type|Clock offset|Spatial equivalent at 40 m/s|
|-|-:|-:|
|Wired Ethernet|< 50 μs|< 2 mm|
|802.11s wireless mesh, software PTP|180–420 μs|approximately 0.7–1.7 cm|
|Practical wireless upper bound|500 μs|2 cm|

These offsets are negligible relative to the meter-level covariance of the fused MTIC estimate.

\---

## 15\. ROS 2 / Fast DDS Configuration

QoS profiles referenced in the manuscript are preserved in:

* `config/qos\_profiles.yaml`
* `config/fastdds.xml`

### 15.1 Example ROS 2 QoS profile structure

```yaml
acoustic\_doa\_alerts:
  reliability: RELIABLE
  history: KEEP\_LAST
  history\_depth: 10
  durability: TRANSIENT\_LOCAL
  deadline\_ms: 100

visual\_bounding\_boxes:
  reliability: BEST\_EFFORT
  history: KEEP\_LAST
  history\_depth: 1
  durability: VOLATILE
  deadline\_ms: 20

mtic\_consensus\_matrices:
  reliability: RELIABLE
  history: KEEP\_LAST
  history\_depth: 5
  durability: VOLATILE
  deadline\_ms: 50
```

### 15.2 Fast DDS small-fragment tuning

The repository includes an XML example that constrains UDP message size for wireless mesh links and helps avoid harmful IP fragmentation in contested or lossy environments.

\---

## 16\. Notes on Supplementary Material

The manuscript’s supplementary package includes:

* system architecture overview
* I2S microphone integration details
* MTIC convergence proof sketch
* parameter sensitivity analysis
* posterior reconstruction derivation
* supplementary threat-assessment plots

This repository directly supports those materials through:

* reproducible CSV outputs
* hardware notes
* configuration examples

\---

## 17\. Citation

If you use this repository, please cite the manuscript as:

```bibtex
@article{tsai2026paperB,
  title   = {Distributed Counter-UAV Early Warning: Acoustic-Visual Information Consensus and Fuzzy-Bayesian Threat Assessment},
  author  = {Tsai, Shang-En},
  journal = {Defence Technology},
  year    = {2026},
  note    = {submitted}
}
```

For the upstream estimator, cite the companion Part I manuscript separately.

\---

## 18\. Recommended Future Extensions

Recommended next steps include:

* replace CSV-only reproduction with real ROS 2 nodes
* connect real Part-I local-state packets into the MTIC layer
* add live DDS topic publishers / subscribers
* add full JPDA or MHT upstream association implementation
* add hardware-timestamped PTP via external NICs
* add forward-error-corrected transport for high-loss wireless mesh
* provide plotting scripts for all CSV outputs

\---

## 19\. Summary

This repository is the **reproducible-result and deployment-note companion** for the submitted Paper B manuscript. It preserves:

* the MTIC fusion layer
* the fuzzy–Bayesian decision layer
* contested-network and HIL evaluation outputs
* Jetson / I2S / PTP / ROS 2 / Fast DDS deployment context

It is intended to support manuscript submission, reviewer clarification, later artifact release, and future transition toward a full deployed distributed C-UAV runtime.

