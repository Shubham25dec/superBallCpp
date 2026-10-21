#pragma once

#include "pch.hpp"


using PolyPoints = std::vector<sf::Vector2f>;

constexpr auto SOLID_V2F = sf::Vector2f(-1.0f, -1.0f);

constexpr auto SHAPE_FILL = sf::Color(200, 207, 255);
constexpr auto SHAPE_OUT  = sf::Color(10, 10, 10);
//TODO: TRY DIFFERENT COLORS

constexpr float ROUND_RADIUS = 6.0f;  // max corner radius
constexpr int   ARC_SEGMENTS = 6;      // points per rounded corner



class ConvexPolygon : public sf::ConvexShape {
public:
    bool is_mortal = true;
    const PolyPoints* sharp_poly_points; //store pointer to original poly data,
    //this is used by ball to check for collision
    // instead of using getPoint() we access vertex from this array directly
    // that is because of the rounded corner
    // rounded corner adds more points to polygon, but they dont necessarily need to be considered during
    // collision check!, we check with the original vertices (without rounded corner) and draw with rounded corner.


    //NOTE: Ensure that PolyPoints always pass a convex
    // polygon. a concave polygon would break rendering
    // and physics of the game!
    ConvexPolygon(const PolyPoints& a_points): sharp_poly_points(&a_points)
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


private:
    static PolyPoints roundCorners(const PolyPoints& src, float radius, int segs) {
        const size_t n = src.size();
        if (n < 3 || radius <= 0.f)
            return src;

        auto len  = [](sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); };
        auto norm = [&](sf::Vector2f v) { float l = len(v); return l > 0.f ? v / l : v; };

        PolyPoints out;
        out.reserve(n * (segs + 1));

        for (size_t i = 0; i < n; ++i) {
            sf::Vector2f p0 = src[(i + n - 1) % n];
            sf::Vector2f p1 = src[i];
            sf::Vector2f p2 = src[(i + 1) % n];

            sf::Vector2f e1 = p0 - p1, e2 = p2 - p1;
            float l1 = len(e1), l2 = len(e2);
            if (l1 < 1e-4f || l2 < 1e-4f) { out.push_back(p1); continue; }

            sf::Vector2f v1 = e1 / l1, v2 = e2 / l2;
            float cosT = std::clamp(v1.x * v2.x + v1.y * v2.y, -1.f, 1.f);
            float theta = std::acos(cosT);   // interior angle at this corner

            // nearly straight or degenerate corner: keep as is
            if (theta > 3.1f || theta < 0.05f) { out.push_back(p1); continue; }

            float halfTan = std::tan(theta * 0.5f);
            float r = radius;
            float d = r / halfTan;                       // tangent distance along edges
            float maxD = std::min(l1, l2) * 0.5f;        // don't overlap neighbors
            if (d > maxD) { d = maxD; r = d * halfTan; }

            sf::Vector2f t1 = p1 + v1 * d;
            sf::Vector2f t2 = p1 + v2 * d;
            sf::Vector2f c  = p1 + norm(v1 + v2) * (r / std::sin(theta * 0.5f));

            float a1 = std::atan2(t1.y - c.y, t1.x - c.x);
            float a2 = std::atan2(t2.y - c.y, t2.x - c.x);
            float sweep = a2 - a1;
            const float PI = 3.14159265358979f;
            while (sweep >  PI) sweep -= 2.f * PI;
            while (sweep < -PI) sweep += 2.f * PI;

            for (int k = 0; k <= segs; ++k) {
                float a = a1 + sweep * (static_cast<float>(k) / segs);
                out.emplace_back(c.x + r * std::cos(a), c.y + r * std::sin(a));
            }
        }
        return out;
    }

public:
    void setPoints(const PolyPoints& points) {
        size_t offset = is_mortal ? 0 : 1;   // skip the SOLID_V2F marker
        PolyPoints src(points.begin() + offset, points.end());

        PolyPoints rounded = roundCorners(src, ROUND_RADIUS, ARC_SEGMENTS);

        setPointCount(rounded.size());
        for (size_t i = 0; i < rounded.size(); ++i)
            setPoint(i, rounded[i]);
    }

    //return true if point lies inside shape
    bool contains(sf::Vector2f point) const {
        //convert to local space so position/rotation/scale are handled
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

