#include "pch.h"
#include "CCamera.h"
#include "Start_Camera.h"
#include "CScene.h"
#include "SceneMgr.h"
#include "CameraMgr.h"
#include "CObject.h"
#include "MouseMgr.h"

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void Start_Camera::Update() {
    auto mousePos = MouseMgr::Instance()->getCursorPos();

    constexpr float screenWidth = 1200.0f;
    constexpr float screenHeight = 768.0f;

    constexpr float sensitivityX = 0.007f; // X축 감도
    constexpr float sensitivityY = 0.007f; // Y축 감도
    constexpr float lerpFactor = 0.5f; // LERP 비율 (0.0~1.0, 클수록 빠르게 움직임)

    CScene* cur = SceneMgr::Instance()->getScene();
    CObject& pl = cur->getObject(GROUP_TYPE::PLAYER, 0);

    // 카메라 설정
    CCamera* cam = CameraMgr::Instance()->getMainCamera();

    vec3 plPos = pl.getPosition();
    cam->position = plPos+vec3(0.0f,0.5f,0.0f);

    float deltaX = (mousePos.x - screenWidth / 2) * sensitivityX;
    float deltaY = (mousePos.y - screenHeight / 2) * sensitivityY;

    static float sumx{};
    static float sumy{};

    sumx -= deltaX;
    sumy += deltaY;

    vec3 target(
        plPos.x + sinf(sumx),
        plPos.y + sumy, // Y축 조절
        plPos.z + cosf(sumx)
    );

    cam->target.x = Lerp(cam->target.x, target.x, lerpFactor);
    cam->target.y = Lerp(cam->target.y, target.y, lerpFactor);
    cam->target.z = Lerp(cam->target.z, target.z, lerpFactor);

    MouseMgr::Instance()->setCursorPos(screenWidth / 2, screenHeight / 2);
}
