#include "pch.h"
#include "MouseMgr.h"

MouseMgr::MouseMgr()
    : width(0), height(0), cursorPos(vec2(0.0f, 0.0f)),
    isLeftClick(false), isRightClick(false) {}

MouseMgr::~MouseMgr() {}

vec2 MouseMgr::getCursorPos() const {
    return cursorPos;
}

bool MouseMgr::getLeftClickState() const {
    return isLeftClick;
}

bool MouseMgr::getRightClickState() const {
    return isRightClick;
}

void MouseMgr::handleMouseClick(int button, int state, int x, int y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    cursorPos = vec2(static_cast<float>(x), static_cast<float>(vp[3] - y));

    if (button == GLUT_LEFT_BUTTON) {
        isLeftClick = (state == GLUT_DOWN);
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        isRightClick = (state == GLUT_DOWN);
    }
}

void MouseMgr::handleMouseMove(int x, int y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    cursorPos = vec2(static_cast<float>(x), static_cast<float>(vp[3] - y));
}

void MouseMgr::Init(int w, int h) {
    setWindowSize(w, h);

    // GLUT 이벤트 연결
    glutMouseFunc([](int button, int state, int x, int y) {
        MouseMgr::Instance()->handleMouseClick(button, state, x, y);
        });

    glutPassiveMotionFunc([](int x, int y) {
        MouseMgr::Instance()->handleMouseMove(x, y);
        });

    glutMotionFunc([](int x, int y) {
        MouseMgr::Instance()->handleMouseMove(x, y);
        });
}


void MouseMgr::setCursorPos(float x, float y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    float invertedY = vp[3] - y;

    glutWarpPointer(static_cast<int>(x), static_cast<int>(invertedY));

    cursorPos = vec2(x, y);
}


void MouseMgr::Update() {
    // 별다른 주기적 업데이트 필요 없음
}

void MouseMgr::setWindowSize(int w, int h) {
    width = w;
    height = h;
}
