#pragma once

#include "../graphics/SFMLGraphics.hpp"
#include <vector>
#include <iostream>
#include <math.h>

struct Particle {
    sf::Vector2f current_pos;
    sf::Vector2f old_pos;
    sf::Vector2f acc;
    sf::Color color = sf::Color::Cyan;
    float radius = 10.0f;
    int gridx = 0;
    int gridy = 0;
    int id = 0;
    float mass = 2.0f;

    static constexpr float max_vel = 100.0f;
    static constexpr float max_accel = 100.0f;

    Particle() = default;

    Particle(sf::Vector2f pos, float radi, int x, int y, int i):
        current_pos{pos},
        old_pos{pos},
        acc{0.0f, 0.0f},
        radius{radi},
        gridx{x},
        gridy{y},
        id{i}
    {}

    void updatePosition(float dt) {
        sf::Vector2f displacement = current_pos - old_pos;
        old_pos = current_pos;
        current_pos = current_pos + displacement + acc * (dt * dt);
        acc = {};

        sf::Vector2f velocity = getVelocity();
        float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (speed > max_vel) {
            velocity = (velocity / speed) * max_vel;
            setVelocity(velocity, dt);
        }

        gridx = current_pos.x / 15;
        gridy = current_pos.y / 15;

        // debug std::cout << "Particle " << id << " updated position to (" << current_pos.x << ", " << current_pos.y << ")\n";
    }

    void accelerate(sf::Vector2f new_acc) {
        acc += new_acc; 

        float magnitude = sqrt(acc.x * acc.x + acc.y * acc.y);
        if (magnitude > max_accel) {
            acc = (acc / magnitude) * max_accel;
        }
    }

    void setVelocity(sf::Vector2f vel, float dt) {
        old_pos = current_pos - (vel * dt);
    }

    void addVelocity(sf::Vector2f vel, float dt) {
        old_pos -= vel * dt;
    }

    sf::Vector2f getVelocity() {
        return current_pos - old_pos;
    }
};