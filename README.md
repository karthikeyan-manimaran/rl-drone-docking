# RL Drone Docking — Precision Landing with Deep Reinforcement Learning

> **Autonomous UAV precision landing using DDPG and SAC algorithms in PX4/Gazebo/ROS2 simulation**

---

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Tech Stack](#tech-stack)
- [Repository Structure](#repository-structure)
- [Prerequisites](#prerequisites)
- [Setup & Installation](#setup--installation)
  - [Option A — Docker (Recommended)](#option-a--docker-recommended)
  - [Option B — Manual Setup](#option-b--manual-setup)
- [Building the ROS2 Workspace](#building-the-ros2-workspace)
- [Running the Project](#running-the-project)
  - [Training — SAC](#1-training--sac-soft-actor-critic)
  - [Training — DDPG](#2-training--ddpg-deep-deterministic-policy-gradient)
  - [Testing — SAC](#3-testing--sac-with-gazebo-gui)
  - [Testing — DDPG](#4-testing--ddpg)
  - [Baseline (Classical Controller)](#5-baseline-classical-precision-landing-controller)
  - [Wrapper Test](#6-environment-wrapper-test)
- [Model Export (PyTorch → ONNX)](#model-export-pytorch--onnx)
- [Log Analysis](#log-analysis)
- [Docker Reference](#docker-reference)
- [Key Configuration Notes](#key-configuration-notes)
- [Troubleshooting](#troubleshooting)
- [Acknowledgements](#acknowledgements)

---

## Overview

**RL Drone Docking** is a simulation-based research project that trains a quadrotor drone (Iris model) to perform autonomous precision landing on a docking pad using deep reinforcement learning. The agent is trained entirely in simulation using the PX4 flight stack, Gazebo simulator, and ROS2 middleware, with the goal of precise landing accuracy while minimizing control effort.

Two state-of-the-art off-policy RL algorithms are implemented and compared:

| Algorithm | Type | Exploration | Notes |
|-----------|------|-------------|-------|
| **SAC** (Soft Actor-Critic) | Stochastic | Entropy regularization | Better exploration, more stable training |
| **DDPG** (Deep Deterministic Policy Gradient) | Deterministic | Ornstein-Uhlenbeck noise | Simpler, deterministic policy |

A **classical baseline controller** using IR-Lock marker tracking is also included for comparison against the RL approaches.

---

## System Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                        Docker Container                          │
│                                                                  │
│  ┌───────────────┐    RTPS/UDP     ┌────────────────────────┐   │
│  │  PX4 SITL     │◄──────────────►│  micrortps_agent       │   │
│  │  (Autopilot)  │                │  (DDS Bridge)          │   │
│  │               │                └────────────┬───────────┘   │
│  │  Gazebo Sim   │                             │ ROS2 Topics    │
│  │  (Iris Drone) │                ┌────────────▼───────────┐   │
│  └───────────────┘                │   px4_ros_extended     │   │
│                                   │   (ROS2 Package)       │   │
│                                   │                        │   │
│                                   │  ┌──────────────────┐  │   │
│                                   │  │  RL Environment   │  │   │
│                                   │  │  (env_wrapper.py) │  │   │
│                                   │  └────────┬─────────┘  │   │
│                                   │           │            │   │
│                                   │  ┌────────▼─────────┐  │   │
│                                   │  │  RL Agent         │  │   │
│                                   │  │  sac_agent.py     │  │   │
│                                   │  │  ddpg_agent.py    │  │   │
│                                   │  └──────────────────┘  │   │
│                                   └────────────────────────┘   │
│                                                                  │
│  Managed via tmux sessions and launch shell scripts              │
└──────────────────────────────────────────────────────────────────┘
```

**Communication Flow:**
1. PX4 SITL runs the flight controller and Gazebo physics simulation.
2. `micrortps_agent` bridges PX4 internal uORB topics to ROS2 via RTPS/UDP.
3. The `px4_ros_extended` ROS2 package wraps the environment (observations, rewards, resets) and hosts the RL agents.
4. The RL agent (SAC/DDPG) sends velocity setpoints back to PX4 via ROS2 topics.

---

## Tech Stack

| Component | Version / Details |
|-----------|-------------------|
| **PX4 Autopilot** | Custom fork (`carlo98/PX4-Autopilot`, branch `rcl_except_4`) |
| **ROS2** | Dashing (primary) / Foxy (alternate via `Dockerfile_foxy`) |
| **Gazebo** | Bundled with PX4 SITL |
| **Python** | 3.x |
| **PyTorch** | 2.0 |
| **Docker** | Required for reproducible environment |
| **tmux** | Used by all launch scripts for multi-pane session management |
| **Colcon** | ROS2 build tool |

---

## Repository Structure

```
rl-drone-docking/
│
├── Dockerfile               # Primary image: ROS2 Dashing + PX4 SITL
├── Dockerfile_foxy          # Alternate image: ROS2 Foxy + PX4 SITL
├── run_docker.sh            # Build/run Docker containers
│
├── ros_packages/            # ROS2 source packages (mounted into container)
│   └── (px4_ros_extended and related packages)
│
├── build/                   # Colcon build artifacts (generated)
├── install/                 # Colcon install artifacts (generated)
├── log/                     # Colcon build logs (generated)
│
├── models/                  # Saved RL model checkpoints (.pt files)
│
├── launch_train_sac.sh      # Train with SAC algorithm
├── launch_train_ddpg.sh     # Train with DDPG algorithm
├── launch_test_sac.sh       # Test SAC policy (with Gazebo GUI)
├── launch_test_ddpg.sh      # Test DDPG policy
├── launch_test_wrapper.sh   # Test environment wrapper only
├── launch_baseline.sh       # Run classical IR-Lock baseline controller
│
├── Log Analysis.ipynb       # Jupyter notebook for training curve analysis
└── torch_to_onnx.ipynb      # Notebook to export PyTorch models to ONNX
```

### Key ROS2 Package — `px4_ros_extended`

Located at `/src/shared/ros_packages/px4_ros_extended/` inside the container.

```
px4_ros_extended/
├── src_py/
│   ├── sac_agent.py         # SAC training + inference agent
│   ├── ddpg_agent.py        # DDPG training + inference agent
│   ├── env_wrapper.py       # Gym-like RL environment wrapper
│   ├── rewards.py           # Reward shaping functions
│   └── gazebo_runner.py     # Gazebo launch/reset controller
├── launch/
│   ├── env_train.launch.py  # Training environment launch file
│   └── env.launch.py        # Standard environment launch file
└── src/
    └── baseline_prec_land   # C++ classical precision landing controller
```

---

## Prerequisites

### Hardware / Host Requirements

- **OS:** Ubuntu 20.04 (recommended) or Ubuntu 18.04
- **RAM:** ≥ 8 GB (container is limited to 3 GB; host needs headroom)
- **Storage:** ≥ 20 GB free (Docker image + PX4 build artifacts are large)
- **GPU:** Optional (training runs on CPU; GPU can accelerate neural network inference)
- **Display:** Required for Gazebo GUI (X11 forwarding is set up in `run_docker.sh`)

### Software Requirements

- [Docker](https://docs.docker.com/engine/install/ubuntu/) installed and running
- `xhost` available (usually part of `x11-xserver-utils`)
- `tmux` installed on the host (optional, but needed if running scripts outside Docker)

---

## Setup & Installation

### Option A — Docker (Recommended)

The entire simulation stack (PX4, Gazebo, ROS2) is pre-built inside the Docker image. This is the only supported and reproducible way to run the project.

#### Step 1 — Clone the repository

```bash
git clone https://github.com/karthikeyan-manimaran/rl-drone-docking.git
cd rl-drone-docking
```

#### Step 2 — (Optional) Adjust host path in `run_docker.sh`

Open `run_docker.sh` and update the volume mount path to point to your local clone:

```bash
# Line to update — change the host-side path before the colon (:)
-v /home/host/rl_ws/precision_landing_shaping_RL/shared:/src/shared:rw
```

Replace `/home/host/rl_ws/precision_landing_shaping_RL` with the actual path where you cloned this repo.

#### Step 3 — Build the Docker image and start the container

**Primary (ROS2 Dashing):**
```bash
chmod +x run_docker.sh
./run_docker.sh build
```

**Alternate (ROS2 Foxy):**
```bash
./run_docker.sh build_foxy
```

> ⚠️ **First build takes 20–40 minutes.** It compiles PX4 from source twice (SITL and RTPS targets). Be patient.

#### Step 4 — Start an existing container

After the first build, use `run` to re-enter the container without rebuilding:

```bash
./run_docker.sh run
# or for Foxy:
./run_docker.sh run_foxy
```

---

### Option B — Manual Setup

> Not recommended. Use Docker unless you have a specific reason to set up natively.

If running natively on Ubuntu 20.04:

```bash
# Install ROS2 Foxy
sudo apt update && sudo apt install ros-foxy-desktop -y

# Install PX4 dependencies
sudo apt install python3-colcon-common-extensions python3-vcstool tmux -y

# Clone PX4 (custom fork required)
git clone --branch rcl_except_4 https://github.com/carlo98/PX4-Autopilot.git
cd PX4-Autopilot
HEADLESS=1 make px4_sitl_default gazebo
HEADLESS=1 make px4_sitl_rtps gazebo

# Install Python dependencies
pip3 install torch==2.0 pandas notebook
```

---

## Building the ROS2 Workspace

Run these steps **inside the Docker container**:

```bash
# Source ROS2 environment
source /opt/ros/dashing/setup.bash   # or foxy

# Navigate to the shared workspace
cd /src/shared

# Build all packages
colcon build --symlink-install

# Source the workspace overlay
source install/setup.bash
```

To build only the RL package:

```bash
colcon build --symlink-install --packages-select px4_ros_extended
source install/setup.bash
```

---

## Running the Project

All launch scripts use **tmux** to manage multiple processes in split panes. Run them from inside the Docker container (`/src/shared` directory, after sourcing the workspace).

```bash
# Always source the workspace first inside the container
source /src/shared/install/setup.bash
```

---

### 1. Training — SAC (Soft Actor-Critic)

Launches the SAC agent and the training environment side-by-side in a tmux session.

```bash
chmod +x launch_train_sac.sh
./launch_train_sac.sh
```

**What it does:**
- **Pane 1 (left):** Starts `sac_agent.py` — initializes the SAC neural networks, replay buffer, and begins collecting experience.
- **Pane 2 (right):** Launches `env_train.launch.py` — starts PX4 SITL + `micrortps_agent` + the environment wrapper node.

Model checkpoints are saved to the `models/` directory periodically.

**Manual equivalent:**
```bash
# Terminal 1 — Start SAC agent
python3 /src/shared/ros_packages/px4_ros_extended/src_py/sac_agent.py

# Terminal 2 — Start training environment (after 3 seconds)
ros2 launch px4_ros_extended env_train.launch.py
```

---

### 2. Training — DDPG (Deep Deterministic Policy Gradient)

```bash
chmod +x launch_train_ddpg.sh
./launch_train_ddpg.sh
```

**What it does:**
- **Pane 1 (left):** Starts `ddpg_agent.py` as a ROS2 node with sim-time enabled. It also internally spawns `micrortps_agent`.
- **Pane 2 (right):** Launches `env_train.launch.py` after a 3-second delay.

**Manual equivalent:**
```bash
# Terminal 1 — Start DDPG agent as ROS2 node
ros2 run px4_ros_extended ddpg_agent.py --ros-args --remap /use_sim_time:=true

# Terminal 2 — Start training environment (after 3 seconds)
ros2 launch px4_ros_extended env_train.launch.py
```

---

### 3. Testing — SAC (with Gazebo GUI)

Runs a trained SAC policy in evaluation mode with the Gazebo visual simulation open.

```bash
chmod +x launch_test_sac.sh
./launch_test_sac.sh
```

**What it does:**
- **Window 1, Pane 1:** Starts `sac_agent.py` in test/inference mode.
- **Window 1, Pane 2:** Launches the training environment node.
- **Window 2:** Opens Gazebo with the GUI (`--no-headless` flag) for visual observation.

---

### 4. Testing — DDPG

```bash
chmod +x launch_test_ddpg.sh
./launch_test_ddpg.sh
```

Runs the DDPG policy in the same split-pane layout as training, but the agent loads a saved checkpoint and does not update weights.

---

### 5. Baseline (Classical Precision Landing Controller)

Runs the traditional IR-Lock marker-based precision landing controller for performance comparison.

```bash
chmod +x launch_baseline.sh
./launch_baseline.sh
```

**What it does:**
- **Window 2:** Starts `micrortps_agent -t UDP` to bridge PX4 topics.
- **Window 1, Pane 1:** Runs the C++ `baseline_prec_land` ROS2 node.
- **Window 1, Pane 2:** Launches `env.launch.py` (standard environment, not training).

---

### 6. Environment Wrapper Test

A utility script to test just the environment wrapper node in isolation (without an RL agent), useful for debugging observation/reward logic.

```bash
chmod +x launch_test_wrapper.sh
./launch_test_wrapper.sh
```

---

## Model Export (PyTorch → ONNX)

Trained PyTorch models can be exported to ONNX format for deployment or cross-framework inference. Use the provided Jupyter notebook:

```bash
# Start Jupyter inside the container
jupyter notebook --ip=0.0.0.0 --no-browser --allow-root

# Open in host browser:
# http://localhost:8888
```

Open `torch_to_onnx.ipynb` and follow the cells. The notebook:
1. Loads a `.pt` checkpoint from `models/`
2. Traces the policy network
3. Exports it to `models/<name>.onnx`

---

## Log Analysis

Training metrics (episode reward, success rate, landing error) are logged to the `log/` directory. Use the provided notebook to visualize training curves:

```bash
jupyter notebook --ip=0.0.0.0 --no-browser --allow-root
```

Open `Log Analysis.ipynb`. The notebook uses `pandas` to load CSV logs and plots:
- Reward per episode over training steps
- Episode length
- Landing precision (distance from target at touchdown)

---

## Docker Reference

| Command | Description |
|---------|-------------|
| `./run_docker.sh build` | Build Dashing image + create + enter container |
| `./run_docker.sh build_foxy` | Build Foxy image + create + enter container |
| `./run_docker.sh run` | Re-enter existing Dashing container |
| `./run_docker.sh run_foxy` | Re-enter existing Foxy container |

**Container names:**
- Dashing: `RL_dashing`
- Foxy: `RL_foxy`

**Volume mounts (inside container):**

| Host Path | Container Path | Access |
|-----------|----------------|--------|
| `./shared/` | `/src/shared/` | Read-Write |
| `/tmp/.X11-unix` | `/tmp/.X11-unix` | Read-Only (Gazebo display) |
| `./.bashrc` | `/root/.bashrc` | Read-Write |

**Memory limit:** The container is capped at **3 GB RAM** (`-m="3g"`). If PX4 compilation fails with OOM errors, either increase this limit or close other processes on the host.

**X11 Display:** `run_docker.sh` automatically calls `xhost +` to allow the container to open Gazebo windows on your host display.

---

## Key Configuration Notes

### ROS2 Distro Variants

| Dockerfile | Base Image | ROS2 | ROS1 Bridge |
|---|---|---|---|
| `Dockerfile` | `px4io/px4-dev-ros-melodic:2021-09-08` | Dashing | Melodic |
| `Dockerfile_foxy` | `px4io/px4-dev-ros-noetic:2021-09-08` | Foxy | Noetic |

The Foxy variant also applies CMake version fixes (patches `CMakeLists.txt` files that require CMake < 3.5) and installs cmake 3.21.4 via pip.

### Custom PX4 Fork

This project uses a **forked PX4** (`carlo98/PX4-Autopilot`, branch `rcl_except_4`) that removes the default "missing RC" failsafe triggered when the drone is placed in offboard control mode without a physical RC transmitter. This is essential for RL training where commands come from the software agent.

### Iris IRLock Model

The Dockerfile replaces the default Gazebo `iris.sdf.jinja`, `iris_irlock.sdf`, and `iris_irlock.world` files with custom versions. These enable IR-Lock beacon detection for the baseline controller and modify the simulation environment for precision landing tasks.

### Python Dependencies Inside Container

```
torch==2.0
pandas
notebook
flake8 and pytest extensions (dev/lint tooling)
argcomplete
```

---

## Troubleshooting

**Container exits immediately after `./run_docker.sh build`:**
Check that Docker is running (`sudo systemctl start docker`) and you have sufficient disk space.

**Gazebo window does not appear:**
Ensure `xhost +` was run on the host before starting the container. The `run_docker.sh` script does this automatically, but if you entered the container manually, run it on your host terminal first.

**`micrortps_agent` not found:**
It is compiled as part of the PX4 RTPS build. Source the PX4 environment:
```bash
source /src/PX4-Autopilot/Tools/setup_gazebo.bash /src/PX4-Autopilot /src/PX4-Autopilot/build/px4_sitl_rtps
```

**`colcon build` fails with CMake version error (Foxy image):**
The `Dockerfile_foxy` already patches this automatically. If building manually, run:
```bash
pip3 install cmake==3.21.4 && hash -r
```

**Out of memory during PX4 compilation:**
Increase the Docker memory limit in `run_docker.sh` from `-m="3g"` to `-m="6g"` (or remove the flag entirely to use all available host RAM).

**tmux session already exists:**
Kill the old session before re-running a launch script:
```bash
tmux kill-server
```

**ROS2 nodes not seeing each other:**
Ensure all nodes are sourcing the same workspace overlay and that `ROS_DOMAIN_ID` is consistent (defaults to 0).

---

## Acknowledgements

- Based on the [precision_landing_shaping_RL](https://github.com/carlo98/precision_landing_shaping_RL) framework by carlo98.
- PX4 Autopilot fork: [carlo98/PX4-Autopilot](https://github.com/carlo98/PX4-Autopilot) (`rcl_except_4` branch).
- Docker base images from [PX4/PX4-containers](https://github.com/PX4/PX4-containers).
- Soft Actor-Critic algorithm: Haarnoja et al., 2018.
- DDPG algorithm: Lillicrap et al., 2015.

---

*Author: Karthikeyan Manimaran | Reg. No. 23BRS1428*
