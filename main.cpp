#include "pch.h"
#include "CCore.h"
#include "Model.h"


static GLvoid Render() {
    CCore::Instance()->Render();
}
static GLvoid Reshape(int w, int h) {
    CCore::Instance()->Reshape(w, h);
}
static GLvoid Update() {
    CCore::Instance()->Update();
}
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>> modelPairs;
std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>>&  Model::modelPairArr = modelPairs;
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(300, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Application");
    glewExperimental = GL_TRUE;
    glewInit();

    static GLuint vertexShader, fragmentShader;
    make_shaderProgram(
        CCore::Instance()->shaderProgramID,
        vertexShader,
        fragmentShader,
        "vertex.glsl",
        "fragment.glsl"
    );
    static GLuint vertexShader2, fragmentShader2;
    make_shaderProgram(
        CCore::Instance()->shaderProgramID2,
        vertexShader2,
        fragmentShader2,
        "animVertex.glsl",
        "animFragment.glsl"
    );
    modelPairs = {
        {MODEL_TYPE::JETT, ReadModel("jettTest")},
        {MODEL_TYPE::ASCENT, ReadModel("ascentB")},
    };
    CCore::Instance()->Init();

    

    

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glutDisplayFunc(Render);
    glutIdleFunc(Update);
    glutReshapeFunc(Reshape);

    glutMainLoop();
}