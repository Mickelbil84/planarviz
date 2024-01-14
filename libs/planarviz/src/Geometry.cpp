#include "Geometry.h"
using namespace planarviz;

#include <gl/glew.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <CGAL/Polygon_2.h>

#include <boost/property_map/property_map.hpp>
typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Constrained_triangulation_face_base_2<K>            Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Face_handle                                          Face_handle;
typedef CDT::Point                                                Point_2;
typedef CGAL::Polygon_2<K>                                        Polygon_2;


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
    glDrawArrays(m_bLines ? GL_LINE_STRIP : GL_TRIANGLES, 0, m_vertices.size());
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

Arrow::Arrow(float length, float headLength, float headWidth) :
    LineStrip(std::vector<Point>()) {
    m_vertices.push_back(Point(0.f, 0.f));
    m_vertices.push_back(Point(length, 0.f));
    m_vertices.push_back(Point(length - headLength, headWidth));
    m_vertices.push_back(Point(length, 0.f));
    m_vertices.push_back(Point(length - headLength, -headWidth));
    createBuffer();
}

Polygon::Polygon(std::vector<Point> vertices, float boundaryThickness) :
    TriangleSoup(std::vector<Point>()) {
    // Generate the local thickness geometry (of triangles)
    for (int idx = 0; idx < vertices.size(); idx++) {
        m_originalVertices.push_back(vertices[idx]);
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

PolygonFill::PolygonFill(std::vector<Point> vertices) : TriangleSoup(std::vector<Point>()) {
    Polygon_2 polygon;
    for (int idx = 0; idx < vertices.size() - 1; idx++)
        polygon.push_back(Point_2(vertices[idx].x, vertices[idx].y));
    
    CDT cdt;
    cdt.insert_constraint(polygon.vertices_begin(), polygon.vertices_end(), true);

    std::unordered_map<Face_handle, bool> in_domain_map;
    boost::associative_property_map<std::unordered_map<Face_handle, bool>> in_domain(in_domain_map);
    CGAL::mark_domain_in_triangulation(cdt, in_domain);

    for (Face_handle f : cdt.finite_face_handles()) {
        if (!get(in_domain, f)) continue;
        for (int i = 0; i < 3; i++) {
            auto v = f->vertex(i);
            m_vertices.push_back(Point(v->point().x(), v->point().y()));
        }
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

Square::Square(float width, float height, bool centered) : TriangleSoup(std::vector<Point>()), m_width(width), m_height(height) {
    Point v1(0.f, 0.f, 0.f, 0.f);
    Point v2(m_width, 0.f, 1.f, 0.f);
    Point v3(m_width, m_height, 1.f, 1.f);
    Point v4(0.f, m_height, 0.f, 1.f);
    m_vertices.push_back(v1); m_vertices.push_back(v2); m_vertices.push_back(v3);
    m_vertices.push_back(v1); m_vertices.push_back(v3); m_vertices.push_back(v4);

    if (centered) {
        for (auto& vertex : m_vertices) {
            vertex.x -= .5f * m_width;
            vertex.y -= .5f * m_height;
        }
    }

    createBuffer();
}