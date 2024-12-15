#include "pch.h"
#include "MouseMgr.h"

MouseMgr::MouseMgr()
    : width(0), height(0), cursorPos(vec2(0.0f, 0.0f)) {}  // vec2로 변경

MouseMgr::~MouseMgr() {}

vec2 MouseMgr::getCursorPos(MOUSE_TYPE type) const {
    if (type == MOUSE_TYPE::LAST) {
        return cursorPos;  // 기본적으로 마지막 좌표 반환
    }
    for (const auto& mouseInfo : arrPos[static_cast<int>(type)]) {
        if (mouseInfo.clicked) {
            return mouseInfo.cursorPos;  // 클릭된 좌표 반환
        }
    }
    return cursorPos;  // 기본적으로 현재 커서 위치 반환
}

void MouseMgr::handleMouseEvent(MOUSE_TYPE event, int x, int y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Y 좌표는 화면 좌상단 기준으로 되어 있으므로, 이를 변환
    arrPos[static_cast<int>(event)].emplace_back(MouseInfo {
        vec3(static_cast<float>(x), static_cast<float>(vp[3] - y), 0.0f),  // vec3로 저장
            true
    });
}

vec2 MouseMgr::getMouseClick(MOUSE_TYPE event) {
    if (!arrPos[static_cast<int>(event)].empty()) {
        return arrPos[static_cast<int>(event)].back().cursorPos;
    }
    return vec2(0.0f, 0.0f); // 기본 값
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