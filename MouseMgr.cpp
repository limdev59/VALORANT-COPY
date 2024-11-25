#include "pch.h"
#include "MouseMgr.h"

MouseMgr::MouseMgr()
    : width(0), height(0), clickDownPos(0.0f), clickUpPos(0.0f)
{
    clicked = false;
}

MouseMgr::~MouseMgr() {}

void MouseMgr::setClickDownPos(int x, int y) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    //cout << "x:" << x << " y:" << y << '\n';
    MouseMgr::Instance()->clicked = false;
}

void MouseMgr::setClickUpPos(int x, int y) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    MouseMgr::Instance()->clicked = true;
}

glm::vec3 MouseMgr::getClickDownPos() const
{
    return clickDownPos;
}

glm::vec3 MouseMgr::getClickUpPos() const
{
    return clickUpPos;
}

void MouseMgr::MouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        MouseMgr::Instance()->setClickDownPos(x, y);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        MouseMgr::Instance()->setClickUpPos(x, y);
    }
}

void MouseMgr::Init(int w, int h/*, Camera& cam*/) {
    MouseMgr::Instance()->SetWindowSize(w, h);
    glutMouseFunc(MouseMgr::MouseCallback);
}

void MouseMgr::Update(/*Camera& cam*/) {
    if (clicked) {
        /*camera = cam;*/
    }
}

void MouseMgr::SetWindowSize(int w, int h) {
    width = w;
    height = h;
}

bool MouseMgr::WasClicked() {
    bool wasClicked = clicked;
    clicked = false;
    return wasClicked;
}
