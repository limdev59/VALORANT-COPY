#include "pch.h"
#include "MouseMgr.h"

MouseMgr::MouseMgr()
    : width(0), height(0), cursorPos(vec2(0.0f, 0.0f)) {}  // vec2�� ����

MouseMgr::~MouseMgr() {}

vec2 MouseMgr::getCursorPos(MOUSE_TYPE type) const {
    if (type == MOUSE_TYPE::LAST) {
        return cursorPos;  // �⺻������ ������ ��ǥ ��ȯ
    }
    for (const auto& mouseInfo : arrPos[static_cast<int>(type)]) {
        if (mouseInfo.clicked) {
            return mouseInfo.cursorPos;  // Ŭ���� ��ǥ ��ȯ
        }
    }
    return cursorPos;  // �⺻������ ���� Ŀ�� ��ġ ��ȯ
}

void MouseMgr::handleMouseEvent(MOUSE_TYPE event, int x, int y) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Y ��ǥ�� ȭ�� �»�� �������� �Ǿ� �����Ƿ�, �̸� ��ȯ
    arrPos[static_cast<int>(event)].emplace_back(MouseInfo {
        vec3(static_cast<float>(x), static_cast<float>(vp[3] - y), 0.0f),  // vec3�� ����
            true
    });
}

vec2 MouseMgr::getMouseClick(MOUSE_TYPE event) {
    if (!arrPos[static_cast<int>(event)].empty()) {
        return arrPos[static_cast<int>(event)].back().cursorPos;
    }
    return vec2(0.0f, 0.0f); // �⺻ ��
}

void MouseMgr::Init(int w, int h) {
    setWindowSize(w, h);
    for (int i = 0; i < static_cast<int>(MOUSE_TYPE::LAST); ++i) {
        arrPos[i].clear();
    }

    // GLUT �̺�Ʈ ����
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
    // �ʿ��� ������Ʈ ���� �߰�
}

void MouseMgr::setWindowSize(int w, int h) {
    width = w;
    height = h;
}