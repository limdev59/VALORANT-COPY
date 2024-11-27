#include "pch.h"
#include "CCore.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"

#include "CObject.h"
#include "CCamera.h"


CCore::CCore() {}

CCore::~CCore() {}

GLvoid CCore::Reshape(int w, int h) {
    MouseMgr::Instance()->setWindowSize(w, h);
    CCore::Instance()->width = w;
    CCore::Instance()->height = (h == 0) ? 1 : h;
    glViewport(0, 0, w, (h == 0) ? 1 : h);
    CameraMgr::Instance()->getMainCamera()->setAspectRatio((float)width, (float)height);
}

GLvoid CCore::Init() {
    TimeMgr::Instance()->Init();
    CameraMgr::Instance()->Init();
    KeyMgr::Instance()->Init();
    SceneMgr::Instance()->Init();
    MouseMgr::Instance()->Init(WINDOW_WIDTH, WINDOW_HEIGHT);
}

GLvoid CCore::Update() {
    TimeMgr::Instance()->Update();
    CameraMgr::Instance()->Update();
    KeyMgr::Instance()->Update();
    SceneMgr::Instance()->Update();
    MouseMgr::Instance()->Update();
    glutPostRedisplay();
}

// ·»´õ¸µ ÇÔ¼ö
void CCore::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    mat4 projection = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
    mat4 view       = CameraMgr::Instance()->getMainCamera()->getViewMatrix();

    GLuint projectionLocation = glGetUniformLocation(shaderProgramID, "projection");
    GLuint viewLocation = glGetUniformLocation(shaderProgramID, "view");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    TimeMgr::Instance()->Render();
    SceneMgr::Instance()->Render();
    glutSwapBuffers();
}

