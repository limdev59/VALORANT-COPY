#define STB_IMAGE_IMPLEMENTATION
#include "pch.h"
#include "CCore.h"
#include "Model.h"
#include "AnimModel.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"
#include "SceneMgr.h"
#include "CObject.h"
#include "CScene.h"
#include "MouseMgr.h"
#include "ClientNetwork.h"

ClientNetwork* g_pNetwork = nullptr;

glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
    return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}
static GLvoid Render() {
    CCore::Instance()->Render();

    glutSwapBuffers();
}
static GLvoid Reshape(int w, int h) {
    CCore::Instance()->Reshape(w, h);
}
static GLvoid Update() {
    if (g_pNetwork) {
        g_pNetwork->PollIncomingPackets();
    }
    CCore::Instance()->Update();
}
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>> modelPairs;
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>>&  Model::modelPairArr = modelPairs;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Application");
    glewExperimental = GL_TRUE;
    glewInit();

    g_pNetwork = new ClientNetwork();
	// IP는 루프백 임시, 포트는 임의 지정
    if (!g_pNetwork->ConnectToServer("127.0.0.1", 7777, 9001)) {
        std::cout << "[Main] Failed to connect to server." << std::endl;
    }

    static GLuint vertexShader, fragmentShader;
    make_shaderProgram(
        CCore::Instance()->shaderProgramID,
        vertexShader,
        fragmentShader,
        "Shaders/vertex.glsl",
        "Shaders/fragment.glsl"
    );
    static GLuint vertexShader2, fragmentShader2;
    make_shaderProgram(
        CCore::Instance()->shaderProgramID2,
        vertexShader2,
        fragmentShader2,
        "Shaders/animVertex.glsl",
        "Shaders/animFragment.glsl"
    );
    modelPairs = {
        {MODEL_TYPE::JETT, ReadModel("jettTest", "jettTest")},
        {MODEL_TYPE::ASCENT_DECORATION, ReadModel("ascent", "ascent")},
        {MODEL_TYPE::ASCENT_FLOOR, ReadModel("ascentFloor", "ascent")},
        {MODEL_TYPE::ASCENT_WALL, ReadModel("ascentWall", "ascent")},
        {MODEL_TYPE::ASCENT_PROPS, ReadModel("ascentProps", "ascent")},
    };


    CCore::Instance()->Init();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutIdleFunc(Update);
    glutDisplayFunc(Render);
    glutReshapeFunc(Reshape);

    glutMainLoop();

    if (g_pNetwork) {
        delete g_pNetwork;
        g_pNetwork = nullptr;
    }
}

