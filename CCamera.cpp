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
    if (height == 0.0f) {
        aspectRatio = 1.0f;
    }
    else {
        aspectRatio = width / height;
    }
}
