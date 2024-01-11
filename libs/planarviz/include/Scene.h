#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Geometry.h"
#include "Constants.h"

namespace planarviz {

    constexpr float DEFAULT_POLYGON_BOUNDARY_THICKNESS = 0.004f;
    constexpr int DEFAULT_CIRCLE_RESOLUTION = 32;
    const Color DEFAULT_HOVER_COLOR(34.f/255.f, 235.f/255.f, 54.f/255.f, 0.8f);

    class Scene {
    public:
        Scene();
        ~Scene();
        void render();
        void updateHover(Point cursor);
        
        void addShader(std::string name, std::string filename);
        void addShader(std::string name, std::string vertexShaderSource, std::string fragmentShaderSource);
        
        void addTexture(std::string name, int width, int height);
        void addTexture(std::string name, std::string filename);

        inline ShaderPtr getShader(std::string name) { return m_pShaders[name]; }
        inline TexturePtr getTexture(std::string name) { return m_pTextures[name]; }

    
        IGeometryPtr addPolygon(std::string name, std::string shaderName, std::vector<Point> vertices, float boundaryThickness = DEFAULT_POLYGON_BOUNDARY_THICKNESS);
        IGeometryPtr addPolygon(std::string name, std::string shaderName, std::string filename, float boundaryThickness = DEFAULT_POLYGON_BOUNDARY_THICKNESS);
        IGeometryPtr addCircle(std::string name, std::string shaderName, float radius, int resolution = DEFAULT_CIRCLE_RESOLUTION);
        IGeometryPtr addHoverableCircle(std::string name, std::string shaderName, float radius, int resolution = DEFAULT_CIRCLE_RESOLUTION, Color hoverColor = DEFAULT_HOVER_COLOR);
        IGeometryPtr addTexturedSquare(std::string name, std::string shaderName, std::string textureName, float width, float height);

        inline IGeometryPtr getObject(std::string name) { return m_pObjects[name]; }
        inline IGeometryPtr operator[](std::string name) { return this->getObject(name); }

        inline IHoverablePtr getHovered() { return m_currentHovered; }
        inline IGeometryPtr getHoveredGeometry() { return m_currentHoveredGeometry; }

        inline void updateAspectRatio(float aspectRatio) { m_camera.setAspectRatio(aspectRatio); }

        inline Camera& getCamera() { return m_camera; }
    
    protected:
        std::map<std::string, IGeometryPtr> m_pObjects;
        std::map<std::string, ShaderPtr> m_pShaders;
        std::map<std::string, TexturePtr> m_pTextures;
        std::map<std::string, std::vector<std::string>> m_shaderObjectMapping;
        std::map<std::string, std::string> m_objectTextureMapping;

        IGeometryPtr m_currentHoveredGeometry = nullptr;
        IHoverablePtr m_currentHovered = nullptr;

        Camera m_camera;

        IGeometryPtr addGeometry(std::string name, std::string shaderName, IGeometryPtr pGeometry);

        void applyObjectTransform(ShaderPtr pShader, SE2 transform);
        void applyCamera(ShaderPtr pShader);

        static std::vector<Point> loadPolygonFromJSON(std::string filename);
    };
    
}