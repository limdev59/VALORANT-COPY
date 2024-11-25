#include "pch.h"
#include "Camera.h"
#include "KeyMgr.h"

Camera::Camera()
    //:position(), target(), up(), fov(), aspectRatio(), nearClip(), farClip()
{
}
Camera::~Camera(){
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    if (projectionMode == ProjectionMode::Perspective) {
        return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    }
    else {
        float orthoSize = 2.0f; // 직각 투영 크기
        return glm::ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, nearClip, farClip);
    }
}

void Camera::Init() {
    position = glm::vec3(0.0f, 2.0f, 2.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    fov = 45.0f;
    aspectRatio = 1.0f;
    nearClip = 0.1f;
    farClip = 100.0f;
    projectionMode = ProjectionMode::Perspective;
}


void Camera::Update() {
    static int ver{ 0 };
    /*if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD || KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::TAP) {
        ++ver;
        if (ver == 10)
            cout << 10;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD || KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::TAP) {
        --ver;
    }*/

    position.x = 5.0f * cosf(ver * 0.03f);
    position.y = 5.0f * cosf(ver * 0.03f);
    position.z = 5.0f * sinf(ver * 0.03f);

    // 투영 모드 전환 처리
    static bool isKeyPressed = false;
    if (KeyMgr::Instance()->getKeyState(KEY::P) == KEY_TYPE::TAP && !isKeyPressed) {
        isKeyPressed = true;
        if (projectionMode == ProjectionMode::Perspective) {
            projectionMode = ProjectionMode::Orthographic;
        }
        else {
            projectionMode = ProjectionMode::Perspective;
        }
    }
    else if (KeyMgr::Instance()->getKeyState(KEY::P) == KEY_TYPE::AWAY) {
        isKeyPressed = false;
    }
}

void Camera::SetAspectRatio(float width, float height) {
    aspectRatio = width / height;
}
