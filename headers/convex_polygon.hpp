#pragma once

#include "pch.hpp"
#include "utils.hpp"


using PolyPoints = std::vector<sf::Vector2f>;

#define SOLID_V2F sf::Vector2f(-1.0f, -1.0f)

constexpr auto SHAPE_FILL = sf::Color::Green;
constexpr auto SHAPE_OUT  = sf::Color::Black;
//TODO: TRY DIFFERENT COLORS


class ConvexPolygon : public sf::ConvexShape {
public:
    bool is_mortal = true;
    ConvexPolygon(const PolyPoints& a_points)
    {
        if (a_points[0] == SOLID_V2F){
            is_mortal = false; //important to set this 1st here
            setFillColor(SHAPE_OUT);
            setOutlineColor(SHAPE_FILL);
            //reversed color in this case
            // since polygon doesnt die
        }else{
            setFillColor(SHAPE_FILL);
            setOutlineColor(SHAPE_OUT);
        }
        //setOutlineThickness(5);
        setPoints(a_points);
    }

    void setPoints(const PolyPoints& points) {
        size_t pt_count = is_mortal ? points.size() : points.size()-1;
        setPointCount(pt_count);
        size_t offset = is_mortal ? 0 : 1;
        for (size_t i = 0; i < pt_count; ++i) {
            auto pt = sf::Vector2f(points[i+offset].x, points[i+offset].y);
            setPoint(i, pt);
        }
    }

    //return true if point lies inside shape
    bool contains(sf::Vector2f point) const {
        // Convert to local space so position/rotation/scale are handled
        sf::Vector2f p = getInverseTransform().transformPoint(point);
        const std::size_t n = getPointCount();
        if (n < 3)
            return false;
        bool hasPos = false, hasNeg = false;
        for (std::size_t i = 0; i < n; ++i){
            sf::Vector2f a = getPoint(i);
            sf::Vector2f b = getPoint((i + 1) % n);
            sf::Vector2f edge = b - a;
            sf::Vector2f toP  = p - a;
            float cross = edge.x * toP.y - edge.y * toP.x;
            if (cross > 0.f) hasPos = true;
            if (cross < 0.f) hasNeg = true;
            //mixed signs -> pt is outside
            if (hasPos && hasNeg)
                return false;
        }
        return true;
    }
};


typedef std::vector<ConvexPolygon> Polygons;

