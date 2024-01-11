#include "Scene.h"
using namespace planarviz;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <memory>

Scene::Scene() : m_camera((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT) {

}

Scene::~Scene() {
   clear();
   m_pShaders.clear();
   m_pTextures.clear();
}

void Scene::clear() {
    m_pObjects.clear();
    m_shaderObjectMapping.clear();
    m_objectTextureMapping.clear();
}

void Scene::addShader(std::string name, std::string vertexShaderSource, std::string fragmentShaderSource) {
    m_pShaders[name] = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
    m_shaderObjectMapping[name] = std::vector<std::string>();
}
void Scene::addShader(std::string name, std::string filename) {
    m_pShaders[name] = std::make_shared<Shader>(filename);
    m_shaderObjectMapping[name] = std::vector<std::string>();
}

void Scene::addTexture(std::string name, int width, int height) { 
    m_pTextures[name] = std::make_shared<Texture>(width, height);
}

void Scene::addTexture(std::string name, std::string filename){
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    addTexture(name, width, height);
    m_pTextures[name]->getData().clear();
    std::copy(&data[0], &data[width * height * 4], std::back_inserter(m_pTextures[name]->getData()));
    m_pTextures[name]->update();
}


IGeometryPtr Scene::addGeometry(std::string name, std::string shaderName, IGeometryPtr pGeometry) {
    m_pObjects[name] = pGeometry;
    auto& mapping = m_shaderObjectMapping[shaderName];
    if (std::find(mapping.begin(), mapping.end(), name) == mapping.end()) mapping.push_back(name);
    return m_pObjects[name];
}

IGeometryPtr Scene::addPolygon(std::string name, std::string shaderName, std::vector<Point> vertices, float boundaryThickness) {
    return addGeometry(name, shaderName, std::static_pointer_cast<IGeometry>(std::make_shared<Polygon>(vertices, boundaryThickness)));
}
IGeometryPtr Scene::addPolygon(std::string name, std::string shaderName, std::string filename, float boundaryThickness) {
    std::vector<Point> vertices = loadPolygonFromJSON(filename);
    return addPolygon(name, shaderName, vertices, boundaryThickness);
}

IGeometryPtr Scene::addCircle(std::string name, std::string shaderName, float radius, int resolution) {
    return addGeometry(name, shaderName, std::static_pointer_cast<IGeometry>(std::make_shared<Circle>(radius, resolution)));
}


IGeometryPtr Scene::addSquare(std::string name, std::string shaderName, float width, float height, bool centered)
{
    return addGeometry(name, shaderName, std::static_pointer_cast<IGeometry>(std::make_shared<Square>(width, height, centered)));
}

IGeometryPtr Scene::addHoverableCircle(std::string name, std::string shaderName, float radius, int resolution, Color hoverColor) {
    return addGeometry(name, shaderName, std::static_pointer_cast<IGeometry>(std::make_shared<HoverableCircle>(radius, resolution, hoverColor)));
}


IGeometryPtr Scene::addTexturedSquare(std::string name, std::string shaderName, std::string textureName, float width, float height, bool centered)
{
    IGeometryPtr geometry = addGeometry(name, shaderName, std::static_pointer_cast<IGeometry>(std::make_shared<Square>(width, height, centered)));
    m_objectTextureMapping[name] = textureName;
    return geometry;
}


void Scene::render() {
    for (auto kv : m_shaderObjectMapping) {
        ShaderPtr pShader = m_pShaders[kv.first];
        pShader->use();
        applyCamera(pShader);
        for (auto objectName : kv.second) {
            auto object = m_pObjects[objectName];
            applyObjectTransform(pShader, object->getTransform());
            pShader->uniform("geometryColor", object->getColor());

            pShader->uniform("isHovered", object == m_currentHoveredGeometry);
            if (object == m_currentHoveredGeometry)
                pShader->uniform("hoverColor", m_currentHovered->getHoverColor());

            pShader->uniform("useTexture", false);
            if (m_objectTextureMapping.contains(objectName)) {
                pShader->uniform("useTexture", true);
                m_pTextures[m_objectTextureMapping[objectName]]->use();
            }
            
            object->render();
        }
    }
}

void Scene::updateHover(Point cursor) {
    if (m_currentHovered != nullptr && m_currentHovered->isDragged()) return;
    
    for (auto kv : m_pObjects) {
        IHoverablePtr pHoverable = std::dynamic_pointer_cast<IHoverable>(kv.second);
        if (pHoverable == nullptr || !pHoverable->isHoverable()) continue;
        pHoverable->setHovered(pHoverable->contains(cursor));
        if (pHoverable->isHovered()) {
            if (m_currentHovered != nullptr) m_currentHovered->setHovered(false);
            m_currentHovered = pHoverable;
            m_currentHoveredGeometry = kv.second;
        } 
        else if (m_currentHovered == pHoverable) {
            m_currentHovered = nullptr;
            m_currentHoveredGeometry = nullptr;
        }
    }
}

void Scene::applyObjectTransform(ShaderPtr pShader, SE2 transform) {
    pShader->uniform("position", transform.x, transform.y);
    pShader->uniform("rotation", transform.theta);
    pShader->uniform("depth", transform.depth);
}

void Scene::applyCamera(ShaderPtr pShader) {
    pShader->uniform("camera.position", m_camera.getX(), m_camera.getY());
    pShader->uniform("camera.zoom", m_camera.getZoom());
    pShader->uniform("camera.aspectRatio", m_camera.getAspectRatio());
}

std::vector<Point> Scene::loadPolygonFromJSON(std::string filename) {
    std::vector<Point> vertices;
    json j = json::parse(getFileContent(filename));
    for (auto v : j["vertices"]) {
        Point p(v[0], v[1]);
        vertices.push_back(Point(v[0], v[1]));
    }
    return vertices;
}