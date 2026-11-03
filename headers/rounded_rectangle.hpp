#pragma once

#include "pch.hpp"

class RoundedRectangleShape : public sf::Shape {
public:
    RoundedRectangleShape(sf::Vector2f size = {0, 0}, float radius = 0, unsigned int cornerPointCount = 15) 
        : rrSize(size), rrRadius(radius), rrCornerPointCount(cornerPointCount) {
        setRadius(radius);
    }

    void setSize(const sf::Vector2f& size) { rrSize = size; update(); }
    void setRadius(float radius) { rrRadius = radius; update(); }

    std::size_t getPointCount() const override { return rrCornerPointCount * 4; }

    sf::Vector2f getPoint(std::size_t index) const override {
        if (rrRadius == 0.f) return {0, 0};

        unsigned int cornerIndex = index / rrCornerPointCount;
        unsigned int pointInCorner = index % rrCornerPointCount;
        
        // Map angles sequentially through all four quadrants clockwise
        float deltaAngle = 90.f / (rrCornerPointCount - 1);
        float angle = (cornerIndex * 90.f + pointInCorner * deltaAngle) * 3.14159265f / 180.f;

        sf::Vector2f center;
        switch (cornerIndex) {
            case 0: center = {rrSize
                    .x - rrRadius, rrSize.y - rrRadius}; break; // Bottom-Right
            case 1: center = {rrRadius, rrSize.y - rrRadius}; break;            // Bottom-Left
            case 2: center = {rrRadius, rrRadius}; break;                       // Top-Left
            case 3: center = {rrSize.x - rrRadius, rrRadius}; break;            // Top-Right
        }

        return center + sf::Vector2f(std::cos(angle) * rrRadius, std::sin(angle) * rrRadius);
    }

private:
    sf::Vector2f rrSize;
    float rrRadius;
    unsigned int rrCornerPointCount;
};
