import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import torch.nn.functional as F

LOG_STD_MIN = -20
LOG_STD_MAX = 2


class Actor(nn.Module):
    def __init__(self, state_dim, action_dim):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(state_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 256),
            nn.ReLU()
        )
        self.mean = nn.Linear(256, action_dim)
        self.log_std = nn.Linear(256, action_dim)

    def forward(self, state):
        x = self.net(state)
        mean = self.mean(x)
        log_std = torch.clamp(self.log_std(x), LOG_STD_MIN, LOG_STD_MAX)
        return mean, log_std


class Critic(nn.Module):
    def __init__(self, state_dim, action_dim):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(state_dim + action_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 256),
            nn.ReLU(),
            nn.Linear(256, 1)
        )

    def forward(self, s, a):
        return self.net(torch.cat([s, a], dim=1))


class SAC:
    def __init__(self, state_dim, action_dim, memory, lr, gamma, tau, batch_size):
        self.memory = memory
        self.gamma = gamma
        self.tau = tau
        self.batch_size = batch_size
        self.alpha = 0.2
        self.action_dim = action_dim

        self.actor = Actor(state_dim, action_dim)
        self.critic1 = Critic(state_dim, action_dim)
        self.critic2 = Critic(state_dim, action_dim)

        self.target_critic1 = Critic(state_dim, action_dim)
        self.target_critic2 = Critic(state_dim, action_dim)

        self.target_critic1.load_state_dict(self.critic1.state_dict())
        self.target_critic2.load_state_dict(self.critic2.state_dict())

        self.actor_opt = optim.Adam(self.actor.parameters(), lr=lr)
        self.critic1_opt = optim.Adam(self.critic1.parameters(), lr=lr)
        self.critic2_opt = optim.Adam(self.critic2.parameters(), lr=lr)

    def select_action(self, state, evaluate=False):
        state = torch.FloatTensor(state).unsqueeze(0)
        mean, log_std = self.actor(state)
        std = log_std.exp()

        if evaluate:
            action = torch.tanh(mean)
        else:
            noise = torch.randn_like(std)
            action = mean + std * noise
            action = torch.tanh(action)

        return action.detach().cpu().numpy()[0]

    def train(self, mem_to_use):

        if self.memory.len() < self.batch_size:
            return

        states, actions, rewards, next_states, dones = self.memory.sample(self.batch_size)

        states = torch.FloatTensor(states)
        actions = torch.FloatTensor(actions)
        rewards = torch.FloatTensor(rewards).unsqueeze(1)
        next_states = torch.FloatTensor(next_states)
        dones = torch.FloatTensor(dones).unsqueeze(1)

        # ---------- TARGET ----------
        with torch.no_grad():
            next_mean, next_log_std = self.actor(next_states)
            next_std = next_log_std.exp()
            noise = torch.randn_like(next_std)
            next_action = next_mean + next_std * noise
            next_action = torch.tanh(next_action)

            log_prob = -((next_action - next_mean) ** 2) / (2 * next_std**2 + 1e-6)
            log_prob = log_prob.sum(dim=1, keepdim=True)

            q1_next = self.target_critic1(next_states, next_action)
            q2_next = self.target_critic2(next_states, next_action)
            q_next = torch.min(q1_next, q2_next)

            target = rewards + self.gamma * (1 - dones) * (q_next - self.alpha * log_prob)

        # ---------- CRITIC ----------
        q1 = self.critic1(states, actions)
        q2 = self.critic2(states, actions)

        loss1 = F.mse_loss(q1, target)
        loss2 = F.mse_loss(q2, target)

        self.critic1_opt.zero_grad()
        loss1.backward()
        self.critic1_opt.step()

        self.critic2_opt.zero_grad()
        loss2.backward()
        self.critic2_opt.step()

        # ---------- ACTOR ----------
        mean, log_std = self.actor(states)
        std = log_std.exp()
        noise = torch.randn_like(std)
        action = mean + std * noise
        action = torch.tanh(action)

        log_prob = -((action - mean) ** 2) / (2 * std**2 + 1e-6)
        log_prob = log_prob.sum(dim=1, keepdim=True)

        q1 = self.critic1(states, action)
        q2 = self.critic2(states, action)
        q = torch.min(q1, q2)

        actor_loss = (self.alpha * log_prob - q).mean()

        self.actor_opt.zero_grad()
        actor_loss.backward()
        self.actor_opt.step()

        # ---------- SOFT UPDATE ----------
        for target_param, param in zip(self.target_critic1.parameters(), self.critic1.parameters()):
            target_param.data.copy_(self.tau * param.data + (1 - self.tau) * target_param.data)

        for target_param, param in zip(self.target_critic2.parameters(), self.critic2.parameters()):
            target_param.data.copy_(self.tau * param.data + (1 - self.tau) * target_param.data)

    def save_models(self, id_file, best=False):
        torch.save(self.actor.state_dict(), f"{id_file}_actor.pth")
        torch.save(self.critic1.state_dict(), f"{id_file}_critic1.pth")
        torch.save(self.critic2.state_dict(), f"{id_file}_critic2.pth")
