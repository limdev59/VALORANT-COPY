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
    
    CCore::Instance()->Update();
}
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>> modelPairs;
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>>&  Model::modelPairArr = modelPairs;

vector<vec3> Model::GetVerticesWorld() {
    vector<vec3> worldVertices;
    for (const auto& group : groups) {
        for (const auto& subMesh : group.subMeshes) {
            // 인덱스를 사용하여 삼각형 구성
            for (size_t i = 0; i < subMesh.indices.size(); ++i) {
                GLuint index = subMesh.indices[i];
                if (index < subMesh.vertices.size()) {
                    // 로컬 좌표를 월드 좌표로 변환
                    glm::vec4 worldPos = transform * glm::vec4(subMesh.vertices[index].position, 1.0f);
                    worldVertices.push_back(glm::vec3(worldPos));
                }
            }
        }
    }
    return worldVertices;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Application");
    glewExperimental = GL_TRUE;
    glewInit();

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

}

