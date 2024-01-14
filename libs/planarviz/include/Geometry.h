#pragma once

#include <vector>
#include <memory>

#include "Color.h"

namespace planarviz {
    struct Point {
        float x, y;
        float u, v;
        Point(float x = 0.f, float y = 0.f, float u = 0.f, float v = 0.f) : x(x), y(y), u(u), v(v) {}
    };

    struct SE2 {
        float x, y, theta;
        float depth = 0.f, scale = 1.f;
        SE2(float x = 0.f, float y = 0.f, float theta = 0.f) : x(x), y(y), theta(theta) {}

        inline void setTranform(float x, float y, float theta) { this->x = x; this->y = y; this->theta = theta; }
        inline void setPosition(float x, float y) { this->x = x; this->y = y;}
        inline void setRotation(float theta) { this->theta = theta; }
        inline void setScale(float scale) { this->scale = scale; } // When possible, it is preffered to not use scaling
        inline void setDepth(float depth) { this->depth = depth; }
    };

    class IHoverable {
    public:
        IHoverable(Color hoverColor) : m_bHoverable(true), m_bHovered(false), m_hoverColor(hoverColor) {}
        virtual ~IHoverable() = default;
        
        inline void setHoverable(bool hoverable) { m_bHoverable = hoverable; }
        inline bool isHoverable() const { return m_bHoverable; }
        inline void setHovered(bool hovered) { m_bHovered = hovered; }
        inline bool isHovered() const { return m_bHovered; }
        inline void setDragged(bool dragged) { m_bDragged = dragged; }
        inline bool isDragged() const { return m_bDragged; }

        inline Color getHoverColor() const { return m_hoverColor; }

        // Requires implementation of wheter a point (cursor) in world-space coords is hovering the object.
        virtual bool contains(Point p) const = 0;


    protected:
        bool m_bHoverable, m_bHovered, m_bDragged;
        Color m_hoverColor;
    };
    using IHoverablePtr = std::shared_ptr<IHoverable>;

    class IGeometry {
    public:
        virtual ~IGeometry() = default;
        virtual void render() = 0;
        inline SE2& getTransform() { return m_transform; }

        inline void setColor(Color color) { m_color = color; }
        inline Color getColor() const { return m_color; }

    protected:
        SE2 m_transform;
        Color m_color;
    };
    using IGeometryPtr = std::shared_ptr<IGeometry>;

    class TriangleSoup : public IGeometry {
    public:
        TriangleSoup(std::vector<Point> vertices);
        virtual ~TriangleSoup();
        virtual void render();

        inline virtual std::vector<Point> getVertices() const { return m_vertices; }
        
    protected: 
        std::vector<Point> m_vertices;
        unsigned int m_vao;
        bool m_bLines = false; // draw a line strip instead of triangles

        void createBuffer();
    };
    class LineStrip : public TriangleSoup {
    public:
        LineStrip(std::vector<Point> vertices) : TriangleSoup(vertices) {m_bLines = true;}
    };

    class Arrow : public LineStrip {
    public:
        Arrow(float length, float headLength, float headWidth);
    };

    class Polygon : public TriangleSoup {
    public:
        Polygon(std::vector<Point> vertices, float boundaryThickness);

        inline virtual std::vector<Point> getVertices() const { return m_originalVertices; }
    
    protected:
        std::vector<Point> m_originalVertices;
    };

    class PolygonFill : public TriangleSoup {
    public:
        PolygonFill(std::vector<Point> vertices);
    };

    class Circle : public TriangleSoup {
    public:
        Circle(float radius, int resolution);
    
    protected:
        float m_radius;
        int m_resolution;
    };

    class Square : public TriangleSoup {
    public:
        Square(float width, float height, bool centered = false);

    protected:
        float m_width, m_height;
    };

    class HoverableCircle : public Circle, public IHoverable {
    public:
        HoverableCircle(float radius, int resolution, Color hoverColor) : Circle(radius, resolution), IHoverable(hoverColor) {}
        virtual bool contains(Point p) const {
            return (p.x - m_transform.x) * (p.x - m_transform.x) + (p.y - m_transform.y) * (p.y - m_transform.y) <= m_radius * m_radius;
        }
    };
}