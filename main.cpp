#include <iostream>
#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "renderer.hpp"
#include "solver.hpp"

int main() {
    constexpr int32_t window_width = 840;
    constexpr int32_t window_height = 840;

    const float        radius         = 2.0f;
    const int          max_objects    = 100;
    const sf::Vector2f spawn_position = {4.0f, 4.0f};
    const float        spawn_velocity = 600.0f;
    const int          max_spawner    = 24;  
    int                num_spawner    = 24;
    int                spawned_count  = 0;

    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "SFML works!");
    
    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);
    Solver solver;
    Renderer renderer{window};

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        int num_objects = solver.getObjects().size();
        if (num_objects < max_objects) {
            spawned_count++;
            for (int i = 0; i < std::min(num_spawner, max_objects - num_objects); i++) {
                auto& new_object = solver.addObject(spawn_position + sf::Vector2f{0.0f, i * 8.0f}, radius);
                solver.setObjectVelocity(new_object, spawn_velocity * sf::Vector2f{0.8, 0.6});
            }
            if (spawned_count / 50 >= num_spawner && num_spawner < max_spawner) num_spawner++;
        }
        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
        window.display();
    }

    return 0;
}