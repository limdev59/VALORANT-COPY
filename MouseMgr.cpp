#include "pch.h"
#include "MouseMgr.h"

MouseMgr::MouseMgr()
    : width(0), height(0), cursorPos(vec3(0.0f, 0.0f, 0.0f)) {}

MouseMgr::~MouseMgr() {}

vec3 MouseMgr::getCursorPos(MOUSE_TYPE type) const {
    if (!arrPos[static_cast<int>(type)].empty()) {
        return arrPos[static_cast<int>(type)].back().cursorPos;
    }
    return vec3(0.0f, 0.0f, 0.0f); // 기본 값
}

void MouseMgr::handleMouseEvent(MOUSE_TYPE event, int x, int y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    arrPos[static_cast<int>(event)].emplace_back(MouseInfo {
        vec3(0.0f, static_cast<float>(vp[3] - y), static_cast<float>(x)),
            true
    });
}

vec3 MouseMgr::getMouseClick(MOUSE_TYPE event) {
    if (!arrPos[static_cast<int>(event)].empty()) {
        return arrPos[static_cast<int>(event)].back().cursorPos;
    }
    return vec3(0.0f, 0.0f, 0.0f); // 기본 값
}

void MouseMgr::Init(int w, int h) {
    setWindowSize(w, h);
    for (int i = 0; i < static_cast<int>(MOUSE_TYPE::LAST); ++i) {
        arrPos[i].clear();
    }

    // GLUT 이벤트 연결
    glutMouseFunc([](int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN)
                MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::LEFT_CLICK_DOWN, x, y);
            else if (state == GLUT_UP)
                MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::LEFT_CLICK_UP, x, y);
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_DOWN)
                MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::RIGHT_CLICK_DOWN, x, y);
            else if (state == GLUT_UP)
                MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::RIGHT_CLICK_UP, x, y);
        }
        });

    glutPassiveMotionFunc([](int x, int y) {
        MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::MOVE_HOVER, x, y);
        });

    glutMotionFunc([](int x, int y) {
        MouseMgr::Instance()->handleMouseEvent(MOUSE_TYPE::MOVE_HOLD, x, y);
        });
}

void MouseMgr::Update() {
    // 필요한 업데이트 로직 추가
}

void MouseMgr::setWindowSize(int w, int h) {
    width = w;
    height = h;
}
