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

    std::cout << "Mouse Position: " << mousePos.x << ", " << mousePos.y << std::endl;

    constexpr float screenWidth = 1200.0f;
    constexpr float screenHeight = 768.0f;

    constexpr float sensitivityX = 0.007f; // X�� ����
    constexpr float sensitivityY = 0.007f; // Y�� ����
    constexpr float lerpFactor = 0.1f; // LERP ���� (0.0~1.0, Ŭ���� ������ ������)

    CScene* cur = SceneMgr::Instance()->getScene();
    CObject& pl = cur->getObject(GROUP_TYPE::PLAYER, 0);

    // ī�޶� ����
    CCamera* cam = CameraMgr::Instance()->getMainCamera();

    vec3 plPos = pl.getPosition();
    cam->position = plPos;

    float deltaX = (mousePos.x - screenWidth / 2) * sensitivityX;
    float deltaY = (mousePos.y - screenHeight / 2) * sensitivityY;

    static float sumx{};
    static float sumy{};

    sumx -= deltaX;
    sumy += deltaY;

    // ��ǥ Ÿ�� ���
    vec3 target(
        plPos.x + sinf(sumx),
        plPos.y + sumy, // Y�� ����
        plPos.z + cosf(sumx)
    );

    // LERP�� �ε巴�� �̵�
    cam->target.x = Lerp(cam->target.x, target.x, lerpFactor);
    cam->target.y = Lerp(cam->target.y, tanf(target.y), lerpFactor);
    cam->target.z = Lerp(cam->target.z, target.z, lerpFactor);

    MouseMgr::Instance()->setCursorPos(screenWidth / 2, screenHeight / 2);
}
