#include "pch.h"
#include "CCamera.h"
#include "KeyMgr.h"
#include "MouseMgr.h"
#include "TimeMgr.h"

CCamera::CCamera()
    : position(0.0f, 0.0f, 0.0f),
    target(0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(45.0f),
    aspectRatio(1.0f),
    nearClip(0.1f),
    farClip(100.0f),
    projType(PROJ_TYPE::PERSPECTIVE) {}

CCamera::~CCamera() {}

glm::mat4 CCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 CCamera::getProjectionMatrix() const {
    constexpr float orthoSize = 2.0f; // 상수 정의
    if (projType == PROJ_TYPE::PERSPECTIVE) {
        return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    }
    else {
        return glm::ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, nearClip, farClip);
    }
}

void CCamera::Init() {
    position = glm::vec3(-3.0f, 0.0f, 0.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    fov = 45.0f;
    aspectRatio = 1.0f;
    nearClip = 0.1f;
    farClip = 100.0f;
    projType = PROJ_TYPE::PERSPECTIVE;
}
void CCamera::Update() {
    //auto mousePos = MouseMgr::Instance()->getCursorPos(MOUSE_TYPE::MOVE_HOVER);
    //constexpr float screenWidth = 1200.0f;
    //constexpr float screenHeight = 768.0f;

    //float z_ndc = (mousePos.z / screenWidth) * 2.0f - 1.0f;
    //float y_ndc = (mousePos.y / screenHeight) * 2.0f - 1.0f;

    //// NDC 값을 타겟 좌표로 변환
    //target.z = z_ndc * 6.0f;
    //target.y = y_ndc * 7.0f; // y 방향은 반전 필요

    float dt = static_cast<float>(DT); // 상수 가져오기
    float speed{ 3.0f };
    if (KeyMgr::Instance()->getKeyState(KEY::UP) == KEY_TYPE::HOLD) {
        position.x += speed * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::DOWN) == KEY_TYPE::HOLD) {
        position.x -= speed * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::RIGHT) == KEY_TYPE::HOLD) {
        position.z += speed * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::LEFT) == KEY_TYPE::HOLD) {
        position.z -= speed * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD) {
        position.y += speed * dt;
        target.y += speed * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::SHIFT) == KEY_TYPE::HOLD) {
        position.y -= speed * dt;
        target.y -= speed * dt;
    }

    static bool keyPPressed = false;
    if (KeyMgr::Instance()->getKeyState(KEY::P) == KEY_TYPE::TAP && !keyPPressed) {
        keyPPressed = true;
        projType = (projType == PROJ_TYPE::PERSPECTIVE) ? PROJ_TYPE::ORTHOGRAPHIC : PROJ_TYPE::PERSPECTIVE;
    }
    else if (KeyMgr::Instance()->getKeyState(KEY::P) == KEY_TYPE::AWAY) {
        keyPPressed = false;
    }
}

void CCamera::setAspectRatio(float width, float height) {
    aspectRatio = width / height;
}
