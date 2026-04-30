#!/bin/bash

SESSION=$USER

tmux new-session -d -s=$SESSION

tmux new-window -t $SESSION:1 -n 'env + agent'

tmux send-keys "python3 /src/shared/ros_packages/px4_ros_extended/src_py/sac_agent.py" C-m

sleep 3

tmux split-window -h -t $SESSION:1

tmux send-keys "ros2 launch px4_ros_extended env_train.launch.py" C-m

tmux attach-session -t $SESSION:1
