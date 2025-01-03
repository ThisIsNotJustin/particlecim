#pragma once

#include "solver.hpp"
#include <string>

class Renderer {
public:
    explicit
    Renderer(sf::RenderTarget &target_) : target{target_}{}

    void render(Solver &solver) const {
        target.clear(sf::Color::Black);

        sf::CircleShape circle{1.0f};
        circle.setPointCount(32);
        circle.setOrigin({1.0f, 1.0f});

        const auto& objects = solver.getObjects();
        for (const auto& obj : objects) {
            circle.setPosition(obj.current_pos);
            sf::Vector2f radius = {obj.radius, obj.radius};
            circle.setScale(radius);
            circle.setFillColor(obj.color);
            target.draw(circle);
        }
    }

private:
    sf::RenderTarget &target;
};