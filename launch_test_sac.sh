#!/bin/bash

SESSION=$USER

tmux new-session -d -s=$SESSION

# -------- WINDOW 1: SAC AGENT --------
tmux new-window -t $SESSION:1 -n 'SAC Agent'

tmux send-keys "python3 /src/shared/ros_packages/px4_ros_extended/src_py/sac_agent.py" C-m

sleep 3

# -------- WINDOW 2: ENV --------
tmux split-window -h -t $SESSION:1

tmux send-keys "ros2 launch px4_ros_extended env_train.launch.py" C-m

sleep 3

# -------- WINDOW 3: GAZEBO (GUI) --------
tmux new-window -t $SESSION:2 -n 'Gazebo'

tmux send-keys "ros2 run px4_ros_extended gazebo_runner.py --test --no-headless" C-m

# -------- ATTACH --------
tmux attach-session -t $SESSION
