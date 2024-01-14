#define _USE_MATH_DEFINES
#include <math.h>

#include <planarviz.h>
using namespace planarviz;


class PlanarvizDemo : public VisualizationLogic {
public:
    virtual void init();
    virtual void update(float deltaTime);

protected:
    std::vector<float> probs;
    float sigma = 0.1f;
};

int main() {
    PlanarVizApplication app(std::make_unique<PlanarvizDemo>(), true);
    app.run();
    return 0;
}

void PlanarvizDemo::init() {
    m_scene.addTexture("demoBuffer", 128, 128);

    m_scene.addPolygon("room", "default", "../resources/polygons/example.json");
    m_scene.getObject("room")->getTransform().setTranform(-0.5f, -0.5f, 0.f * 3.14159254f / 180.f);
    m_scene.getObject("room")->setColor(Color(1.f, 0.f, 0.f, 1.f));

    m_scene.addPolygonFill("roomFill", "default", "../resources/polygons/example.json");
    m_scene.getObject("roomFill")->getTransform().setTranform(-0.5f, -0.5f, 0.f * 3.14159254f / 180.f);
    m_scene.getObject("roomFill")->getTransform().setDepth(0.3f);
    m_scene.getObject("roomFill")->setColor(Color(.8f, .8f, .8f, .4f));
    
    m_scene.addTexturedSquare("demoBuffer", "default", "demoBuffer", 2.f, 2.f);
    m_scene.getObject("demoBuffer")->getTransform().setPosition(-1.f, -1.f);
    m_scene.getObject("demoBuffer")->getTransform().setDepth(.2f);

    m_scene.addHoverableCircle("exampleCircle", "default", 0.03f);
    m_scene.getObject("exampleCircle")->getTransform().setDepth(-.1f);

    std::vector<Point> strip = {
        Point(0.f, 0.f), Point(1.f, 0.f), Point(.5f, .5f), Point(.7f, -.5f)
    };
    m_scene.addLineStrip("lineStrip", "default", strip);
    m_scene.getObject("lineStrip")->setColor(Color(0.f, 0.f, 1.f, 1.f));

    m_scene.addArrow("arrow", "default");
    m_scene.getObject("arrow")->setColor(Color(1.f, 0.f, 1.f, 1.f));
    m_scene.getObject("arrow")->getTransform().setScale(.1f);

    probs = std::vector<float>(128 * 128, 0.f);
}

void PlanarvizDemo::update(float deltaTime) {

    // ImGui menu
    // ImGui::SetNextWindowPos(ImVec2(m_windowWidth - 300, 0));
    // ImGui::SetNextWindowSize(ImVec2(300, m_windowHeight));
    ImGui::Begin("PlanarViz Demo", nullptr, 0);//ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Text("Normal distribution STD:");
    ImGui::SliderFloat("float", &sigma, 0.0f, 1.0f);
    ImGui::End();
    
    SE2 demoTransform = m_scene.getObject("exampleCircle")->getTransform();
    float coeff = 0.5f * M_2_SQRTPI * M_SQRT1_2;

    m_scene.getObject("arrow")->getTransform().setPosition(demoTransform.x, demoTransform.y);
    
    for (int row = 0; row < 128; row++)
    for (int col = 0; col < 128; col++) {
        float x =  2.f * col / 128.f - 1.f;
        float y =  2.f * row / 128.f - 1.f;
        float dist = (x - demoTransform.x) * (x - demoTransform.x) + (y - demoTransform.y) * (y - demoTransform.y);
        float prob = coeff * expf(-0.5f * dist / (sigma * sigma));
        probs[row * 128 + col] = prob;
    }
    m_scene.getTexture("demoBuffer")->colorizeBuffer(probs, Color(0.f, 0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f, 0.f));
}