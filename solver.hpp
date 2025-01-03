#pragma once

#include <vector>
#include <iostream>
#include "src/graphics/SFMLGraphics.hpp"
#include "src/physics/particle.hpp"

class Solver {
public:
    Solver() = default;

    Particle& addObject(sf::Vector2f pos, float radius) {
        int gridx = pos.x / grid_size;
        int gridy = pos.y / grid_size;

        Particle new_particle = Particle(pos, radius, gridx, gridy, objects.size());
        grid[gridx][gridy].push_back(objects.size());

        return objects.emplace_back(new_particle);
    }

    void update() {
        float substep_dt = step_dt / sub_steps;
        for (int i = 0; i < sub_steps; i++) {
            applyGravity();
            checkCollisions();
            applyBorder();
            updateObjects(substep_dt);
        }
    }

    std::vector<Particle>& getObjects() {
        return objects;
    }

    void setObjectVelocity(Particle &object, sf::Vector2f vel) {
        object.setVelocity(vel, step_dt / sub_steps);
    }

private:
    float window_size = 840.0f;
    sf::Vector2f gravity = {0.0f, 1000.0f};
    sf::Vector2f boundary_center = {420.0f, 420.0f};
    float boundary_radius = 100.0f;
    float step_dt = 1.0f / 60;
    int sub_steps = 8;

    int grid_size = 12;
    std::vector<int> grid[350][350];

    std::vector<Particle> objects;

    void applyGravity() {
        for (auto& obj : objects) {
            obj.accelerate(gravity);
        }
    }

    void applyBorder() {
        for (auto& obj : objects) {
            const float dampening = 0.75f;
            sf::Vector2f npos = obj.current_pos;
            sf::Vector2f vel = obj.getVelocity();
            
            sf::Vector2f dy = {vel.x * dampening, - vel.y};
            sf::Vector2f dx = {-vel.x, vel.y * dampening};

            if (npos.x < obj.radius || npos.x + obj.radius > window_size) {
                npos.x = std::clamp(npos.x, obj.radius, window_size - obj.radius);
                obj.setVelocity({-vel.x * dampening, vel.y}, 1.0f);
                obj.current_pos = npos;
            }

            if (npos.y < obj.radius || npos.y + obj.radius > window_size) {
                npos.y = std::clamp(npos.y, obj.radius, window_size - obj.radius);
                obj.setVelocity({vel.x, dampening * -vel.y}, 1.0f);
                obj.current_pos = npos;
            }
        }
    }

    void applyBoundary() {
        for (auto& obj : objects) {
            sf::Vector2f r = boundary_center - obj.current_pos;
            float dist = sqrt(r.x * r.x + r.y * r.y);

            if (dist > boundary_radius - obj.radius) {
                sf::Vector2f n = r / dist;
                obj.current_pos = boundary_center - n * (boundary_radius - obj.radius);
                sf::Vector2f vel = obj.getVelocity();
                obj.setVelocity(vel - 2.0f * (vel.x * n.x + vel.y * n.y) * n, 1.0f);
            }
        }
    }

    void updateObjects(float dt) {
        for (int i = 0; i < 350; i++) {
            for (int j = 0; j < 350; j++) {
                grid[i][j].clear();
            }
        }

        for (auto& obj : objects) {
            int prev_gridx = obj.gridx;
            int prev_gridy = obj.gridy;

            obj.updatePosition(dt);

            obj.gridx = obj.current_pos.x / grid_size;
            obj.gridy = obj.current_pos.y / grid_size;

            if (prev_gridx != obj.gridx || prev_gridy != obj.gridy) {
                if (prev_gridx >= 0 && prev_gridx < 350 && prev_gridy >= 0 && prev_gridy < 350) {
                    auto& cell = grid[prev_gridx][prev_gridy];
                    cell.erase(std::remove(cell.begin(), cell.end(), obj.id), cell.end());
                }

                if (obj.gridx >= 0 && obj.gridx < 350 && obj.gridy >= 0 && obj.gridy < 350) {
                    grid[obj.gridx][obj.gridy].push_back(obj.id);
                } else {
                    // debug std::cerr << "Error: Particle " << obj.id << " moved out of grid bounds (" << obj.gridx << ", " << obj.gridy << ")\n";
                }
            }
        }
    }

    void checkCollisions() {
        int num_cells = window_size / grid_size;
        int dx[] = {1, 1, 0, 0, -1};
        int dy[] = {0, 1, 0, 1, 1};
        
        for (int i = 0; i < num_cells; i++) {
            for (int j = 0; j < num_cells; j++) {
                if (!grid[i][j].empty()) {
                    for (int k = 0; k < 5; k++) {
                        int nx = i + dx[k];
                        int ny = j + dy[k];
                        if (nx < 0 || ny < 0 || nx >= num_cells || ny >= num_cells)
                            continue;

                        collideCells(i, j, nx, ny);
                    }
                }
            }
        }
    }

    void collideCells(int x1, int y1, int x2, int y2) {
        for (int id1 : grid[x1][y1]) {
            Particle& obj1 = objects[id1];
            for (int id2 : grid[x2][y2]) {
                if (id1 == id2) continue;
                Particle& obj2 = objects[id2];
                if (isCollision(obj1, obj2)) {
                    resolveCollision(obj1, obj2);
                }
            }
        }
    }

    bool isCollision(const Particle& obj1, const Particle& obj2) {
        sf::Vector2f delta = obj1.current_pos - obj2.current_pos;
        float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
        return dist < (obj1.radius + obj2.radius);
    }

    void resolveCollision(Particle& obj1, Particle& obj2) {
        sf::Vector2f delta = obj1.current_pos - obj2.current_pos;
        float dist = sqrt(delta.x * delta.x + delta.y * delta.y);

        if (dist == 0) {
            return;
        }

        sf::Vector2f norm = delta / dist;
        sf::Vector2f relVel = obj1.getVelocity() - obj2.getVelocity();
        float velNorm = relVel.x * norm.x + relVel.y * norm.y;

        if (velNorm > 0) return;

        const float rest = 0.75f;
        float impulse = -(1 + rest) * velNorm;
        impulse /= (1.0f / obj1.mass + 1.0f / obj2.mass);

        sf::Vector2f impulseVec = impulse * norm;
        obj1.setVelocity(obj1.getVelocity() + impulseVec / obj1.mass, 1.0f);
        obj1.setVelocity(obj2.getVelocity() - impulseVec / obj2.mass, 1.0f);
    }
};