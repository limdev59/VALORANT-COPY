#include "pch.h"
#include "CCore.h"
#include "Model.h"
#include "AnimModel.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"



AnimModel* mainModel;
AnimModel* mapModel;
Animator* animator;
Animation* idleAnim;
Animation* runAnim;



glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
    return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}
static GLvoid Render() {
    CCore::Instance()->Render();
   
    glUseProgram(CCore::Instance()->shaderProgramID2);
    {
        AnimModel* currModel = mainModel;
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = currModel->GetModelMat();
        modelMat = glm::scale(modelMat, vec3(0.3f, 0.3f, 0.3f));
        modelMat = glm::rotate(modelMat, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::translate(modelMat, vec3(0.0f, -1.0f, 0.0f));
        mat4 projMat = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
        mat4 view = CameraMgr::Instance()->getMainCamera()->getViewMatrix();
        glm::mat4 PVM = projMat * view * modelMat;
        glm::mat3 normalMat = GetNormalMat(modelMat);
        GLuint loc_modelMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "modelMat");
        GLuint loc_PVM = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "PVM");
        GLuint loc_normalMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMat");
        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));


        const auto& transforms = animator->GetFinalBoneMatrices();
        GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
        for (int i = 0; i < transforms.size(); i++)
            glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

        GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
        GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");

        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);

        mainModel->Render();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
            std::cout << "error : " << error << std::endl;

    }
    glutSwapBuffers();
}
static GLvoid Reshape(int w, int h) {
    CCore::Instance()->Reshape(w, h);
}
static GLvoid Update() {
    CCore::Instance()->Update();
    TimeMgr::Instance()->Update();
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD) {
        for (size_t i{}; i < 3; ++i) {
            if (animator->GetCurrAnimation() != runAnim)
                animator->PlayAnimation(runAnim);
        }
    }
    else {
        for (size_t i{}; i < 3; ++i) {
            if (animator->GetCurrAnimation() != idleAnim)
                animator->PlayAnimation(idleAnim);
        }
    }
    for (size_t i{}; i < 3; ++i) {
        animator->UpdateAnimation(DT);
    }
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

    mainModel = new AnimModel();
    std::string modelPath = "first";
    mainModel->LoadModel(modelPath);
    AnimModel* currModel = mainModel;
    idleAnim = new Animation("Models/first/firstIdle.gltf", currModel);
    runAnim = new Animation("Models/first/first.gltf", currModel);
    animator = new Animator(nullptr);


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