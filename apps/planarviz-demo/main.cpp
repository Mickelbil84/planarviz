#include <WindowApplication.h>
using namespace planarviz;

class LocalizationDemo : public WindowLogic {
public:
    virtual void init();
    virtual void handleInput(WindowInput input);
    virtual void update(float deltaTime);
    virtual void render();
    virtual void shutdown();

protected:
    float dragX, dragY;
    float m_elapsedTime = 0.f;

    std::vector<float> probs;
    float sigma = 0.1f;
};

int main() {
    WindowApplication app(std::make_unique<LocalizationDemo>());
    app.run();
    return 0;
}

void LocalizationDemo::init() {
    m_scene.addShader("basic2d", "../resources/shaders/basic2d");

    m_scene.addTexture("demoBuffer", 128, 128);
    // m_scene.addTexture("demoBuffer", "../resources/textures/bricks.jpg");

    m_scene.addPolygon("room", "basic2d", "../resources/polygons/example.json");
    m_scene.getObject("room")->getTransform().setTranform(-0.5f, -0.5f, 0.f * 3.14159254f / 180.f);
    m_scene.getObject("room")->setColor(Color(1.f, 0.f, 0.f, 1.f));
    
    m_scene.addTexturedSquare("demoBuffer", "basic2d", "demoBuffer", 2.f, 2.f);
    m_scene.getObject("demoBuffer")->getTransform().setPosition(-1.f, -1.f);
    m_scene.getObject("demoBuffer")->getTransform().setDepth(.5f);

    m_scene.addHoverableCircle("exampleCircle", "basic2d", 0.03f);
    m_scene.getObject("exampleCircle")->getTransform().setDepth(-.1f);

    probs = std::vector<float>(128 * 128, 0.f);
}
void LocalizationDemo::handleInput(WindowInput input) {
    // Get cursor from screen space to world coordinates
    Point cursor(input.mouseScreenX, input.mouseScreenY);
    cursor.y /= m_scene.getCamera().getAspectRatio();
    cursor.x -= m_scene.getCamera().getX(); cursor.y -= m_scene.getCamera().getY(); 
    cursor.x /= m_scene.getCamera().getZoom(); cursor.y /= m_scene.getCamera().getZoom();

    if (input.bIsDrag) {
        if (input.bIsDragBegin) {
            if (m_scene.getHoveredGeometry()) {
                dragX = m_scene.getHoveredGeometry()->getTransform().x - cursor.x;
                dragY = m_scene.getHoveredGeometry()->getTransform().y - cursor.y;
                m_scene.getHovered()->setDragged(true);
            }
            else {
                dragX = m_scene.getCamera().getX();
                dragY = m_scene.getCamera().getY();
            }
        }
        float panSpeed = m_scene.getCamera().panSpeed;
        if (!m_bCursorOverride) {
            if (m_scene.getHoveredGeometry())
                m_scene.getHoveredGeometry()->getTransform().setPosition(cursor.x + dragX, cursor.y + dragY);
            else
                m_scene.getCamera().setPosition(dragX + panSpeed * input.dragDX, dragY + panSpeed * input.dragDY);
        }
    }
    else {
        if (m_scene.getHovered()) m_scene.getHovered()->setDragged(false);
        if (!m_bCursorOverride) m_scene.updateHover(cursor);
    }

    if (input.mouseScroll != 0) 
        if (!m_bCursorOverride) m_scene.getCamera().addZoom(input.mouseScroll * m_scene.getCamera().zoomSpeed);

    
    
}
void LocalizationDemo::update(float deltaTime) {

    // ImGui menu
    // ImGui::SetNextWindowPos(ImVec2(m_windowWidth - 300, 0));
    // ImGui::SetNextWindowSize(ImVec2(300, m_windowHeight));
    // ImGui::Begin("Localization Demo", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    // ImGui::Text("Normal distribution STD:");
    // ImGui::SliderFloat("float", &sigma, 0.0f, 1.0f);
    // ImGui::End();

    m_elapsedTime += deltaTime;

    // TexturePtr demoBuffer = m_scene.getTexture("demoBuffer");
    // for (int row = 0; row < demoBuffer->getHeight(); row++) 
    // for (int col = 0; col < demoBuffer->getWidth(); col++)
    // for (int color = 0; color < 4; color ++) {
    //     demoBuffer->getData()[color * demoBuffer->getWidth() * demoBuffer->getHeight() + row * demoBuffer->getWidth() + col] = 
    //         (int)((cosf(row + m_elapsedTime) + sinf(col + m_elapsedTime) + 2.f) * 255.f);
    //         // 0;
    // }
    
    SE2 demoTransform = m_scene.getObject("exampleCircle")->getTransform();
    // float sigma = 0.1f;
    float coeff = 0.5f * M_2_SQRTPI * M_SQRT1_2;
    
    for (int row = 0; row < 128; row++)
    for (int col = 0; col < 128; col++) {
        float x =  2.f * col / 128.f - 1.f;
        float y =  2.f * row / 128.f - 1.f;
        float dist = (x - demoTransform.x) * (x - demoTransform.x) + (y - demoTransform.y) * (y - demoTransform.y);
        float prob = coeff * expf(-0.5f * dist / (sigma * sigma));
        probs[row * 128 + col] = prob;
    }
    m_scene.getTexture("demoBuffer")->colorizeBuffer(probs, Color(0.f, 0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f, 1.f));
}
void LocalizationDemo::render() {
    m_scene.render();
}

void LocalizationDemo::shutdown() {

}