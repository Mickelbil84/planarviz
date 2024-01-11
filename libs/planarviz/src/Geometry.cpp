#include "Geometry.h"
using namespace planarviz;

#include <gl/glew.h>


TriangleSoup::TriangleSoup(std::vector<Point> vertices) {
    if (vertices.size() > 0) {
        m_vertices = vertices;
        createBuffer();
    }
}
TriangleSoup::~TriangleSoup() {
    glDeleteVertexArrays(1, &m_vao);
}
void TriangleSoup::render() {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    glBindVertexArray(0);
}
void TriangleSoup::createBuffer() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Point), &m_vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, x));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, u));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Polygon::Polygon(std::vector<Point> vertices, float boundaryThickness) :
    TriangleSoup(std::vector<Point>()) {
    // Generate the local thickness geometry (of triangles)
    for (int idx = 0; idx < vertices.size(); idx++) {
        Point a = vertices[idx];
        Point b = vertices[(idx + 1) % vertices.size()];
        float len = sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
        Point v((b.x - a.x) * boundaryThickness / len, (b.y - a.y) * boundaryThickness / len); Point n(-v.y, v.x);
        v = Point(0.5*v.x, 0.5*v.y);
        a = Point(a.x - v.x, a.y - v.y); b = Point(b.x + v.x, b.y + v.y);
        Point a_plus(a.x + n.x, a.y + n.y); Point a_minus(a.x - n.x, a.y - n.y);
        Point b_plus(b.x + n.x, b.y + n.y); Point b_minus(b.x - n.x, b.y - n.y);
        m_vertices.push_back(a_minus); m_vertices.push_back(b_plus); m_vertices.push_back(b_minus);
        m_vertices.push_back(a_minus); m_vertices.push_back(b_plus); m_vertices.push_back(a_plus);
    }
    createBuffer();
}

Circle::Circle(float radius, int resolution) : TriangleSoup(std::vector<Point>()), m_radius(radius), m_resolution(resolution) {
    Point origin;
    for (int i = 0; i <= resolution; i++) {
        Point u(m_radius * cosf(2 * i * M_PI / (resolution - 1)), m_radius * sinf(2 * i * M_PI / (resolution - 1)));
        Point v(m_radius * cosf(2 * ((i+1) % resolution) * M_PI / (resolution - 1)), m_radius * sinf(2 * ((i+1) % resolution) * M_PI / (resolution - 1)));
        m_vertices.push_back(origin); m_vertices.push_back(u); m_vertices.push_back(v);
    }
    createBuffer();
}

Square::Square(float width, float height) : TriangleSoup(std::vector<Point>()), m_width(width), m_height(height) {
    Point v1(0.f, 0.f, 0.f, 0.f);
    Point v2(m_width, 0.f, 1.f, 0.f);
    Point v3(m_width, m_height, 1.f, 1.f);
    Point v4(0.f, m_height, 0.f, 1.f);
    m_vertices.push_back(v1); m_vertices.push_back(v2); m_vertices.push_back(v3);
    m_vertices.push_back(v1); m_vertices.push_back(v3); m_vertices.push_back(v4);
    createBuffer();
}