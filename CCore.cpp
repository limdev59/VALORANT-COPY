#include "pch.h"
#include "CCore.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "CObject.h"
#include "MouseMgr.h"

#include "Camera.h"

CCore::CCore() {}

CCore::~CCore() {}

void CCore::Init() {
    Camera::Instance()->Init();
    TimeMgr::Instance()->Init();
    KeyMgr::Instance()->Init();
    SceneMgr::Instance()->Init();
    MouseMgr::Instance()->Init(WINDOW_WIDTH, WINDOW_HEIGHT);
}



void CCore::Update() {
    Camera::Instance()->Update();
    TimeMgr::Instance()->Update();
    KeyMgr::Instance()->Update();
    SceneMgr::Instance()->Update();
    MouseMgr::Instance()->Update();
    glutPostRedisplay();
}

// ·»´õ¸µ ÇÔ¼ö
void CCore::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    mat4 projection = Camera::Instance()->GetProjectionMatrix();
    mat4 view = Camera::Instance()->GetViewMatrix();

    GLuint projectionLocation = glGetUniformLocation(shaderProgramID, "projection");
    GLuint viewLocation = glGetUniformLocation(shaderProgramID, "view");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    TimeMgr::Instance()->Render();
    SceneMgr::Instance()->Render();
    glutSwapBuffers();
}

