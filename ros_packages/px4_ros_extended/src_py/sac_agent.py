#!/usr/bin/env python3

import yaml
import numpy as np
import time
import torch
import threading
import sys
import subprocess

import rclpy

sys.path.append("/src/shared/ros_packages/px4_ros_extended/src_py")

from SAC.sac import SAC
from DDPG.memory import Memory
from env_wrapper import EnvWrapperNode


class AgentNode:
    def __init__(self, node):

        with open('/src/shared/ros_packages/px4_ros_extended/src_py/params.yaml') as info:
            self.info_dict = yaml.load(info, Loader=yaml.SafeLoader)

        torch.manual_seed(self.info_dict['seed'])

        # -------- ENV --------
        self.env = EnvWrapperNode(
            node,
            self.info_dict['obs_shape'],
            self.info_dict['action_space'],
            self.info_dict['max_height'],
            self.info_dict['max_side'],
            self.info_dict['max_vel_z'],
            self.info_dict['max_vel_xy'],
            self.info_dict['eps_pos_xy']
        )

        # -------- MEMORY --------
        self.memory = Memory(self.info_dict['max_memory_len'])

        # -------- SAC --------
        self.sac = SAC(
            self.info_dict['obs_shape'],
            self.info_dict['action_space'],
            self.memory,
            lr=self.info_dict['lr_actor'],
            gamma=self.info_dict['gamma'],
            tau=self.info_dict['tau'],
            batch_size=self.info_dict['batch_size']
        )

        self.prev_action = None

    def train(self):

        episode_tot_reward = 0.0
        episode_num = 0
        episode_steps = 0
        cont_steps = 0
        best_reward = -np.inf

        while self.env.reset:
            pass

        inputs = self.env.play_env()

        while episode_num < self.info_dict['num_env_episodes']:

            episode_steps += 1
            normalized_input = self.normalize_input(np.copy(inputs))

            # -------- WARMUP: random exploration for first 3000 steps --------
            if self.memory.len() < 3000:
                # FIX: wider random range during warmup for better exploration
                action = np.random.uniform(-0.8, 0.8, size=self.info_dict['action_space'])
            else:
                action = self.sac.select_action(normalized_input, evaluate=False)
                # FIX: relaxed clipping — allow larger actions for exploration
                action = np.clip(action, -0.8, 0.8)

            # -------- ACTION SMOOTHING --------
            if self.prev_action is None:
                self.prev_action = action

            # FIX: less smoothing — 0.6/0.4 instead of 0.8/0.2
            # 0.8/0.2 was too smooth, agent moved too slowly and drifted out of bounds
            action = 0.6 * self.prev_action + 0.4 * action
            self.prev_action = action

            inputs, reward, done = self.env.act(action, self.normalize_input)

            previous_obs = np.copy(normalized_input)
            normalized_input = self.normalize_input(np.copy(inputs))

            if episode_steps > 1:
                episode_tot_reward += reward
                self.memory.add(previous_obs, action, reward, normalized_input, done, episode_num)

            # -------- TRAIN: start earlier, train more frequently --------
            if self.memory.len() > 3000:
                self.sac.train(self.info_dict['mem_to_use'])

            if done:
                print(f"Episode: {episode_num} | Steps: {episode_steps} | Reward: {round(episode_tot_reward, 2)} | Memory: {self.memory.len()}")

                self.memory.add_acc_reward(episode_tot_reward, False)

                # Save best model
                if episode_tot_reward > best_reward:
                    best_reward = episode_tot_reward
                    print(f"  >> New best reward: {round(best_reward, 2)} — saving model")
                    self.sac.save_models(str(self.memory.id_file) + "_best")

                self.env.reset_env()
                self.prev_action = None  # FIX: reset action smoothing each episode

                episode_tot_reward = 0.0
                episode_num += 1
                episode_steps = 0

                inputs = self.env.play_env()

            cont_steps += 1

        print("Saving final model...")
        self.sac.save_models(self.memory.id_file)
        self.env.shutdown_gazebo()

    def normalize_input(self, inputs):
        inputs[:2] /= self.info_dict['max_side']
        inputs[2] /= self.info_dict['max_height']
        inputs[3:] /= self.info_dict['max_vel_xy']
        return inputs


def spin_thread(node):
    rclpy.spin(node)


if __name__ == '__main__':
    print("Starting micrortps agent")

    micrortps_agent = subprocess.Popen(["micrortps_agent", "-t", "UDP"])
    time.sleep(2)

    rclpy.init(args=None)
    m_node = rclpy.create_node('agent_node')

    agent = AgentNode(m_node)

    t = threading.Thread(target=spin_thread, args=(m_node,))
    t.start()

    agent.train()

    rclpy.shutdown()
    t.join()
    micrortps_agent.kill()
