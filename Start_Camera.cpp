#include "pch.h"
#include "CCamera.h"
#include "Start_Camera.h"
#include "CScene.h"
#include "SceneMgr.h"
#include "CameraMgr.h"
#include "CObject.h"
#include "MouseMgr.h"

void Start_Camera::Update() {
    // ���콺 ���� ������Ʈ
    MouseMgr::Instance()->Update();

    // �ֽ� ���콺 ��ǥ ��������
    auto mousePos = MouseMgr::Instance()->getCursorPos(MOUSE_TYPE::MOVE_HOVER);

    // ���콺 ��ǥ�� �� �ٲ���� �α׷� Ȯ��
    std::cout << "Mouse Position: " << mousePos.x << ", " << mousePos.y << std::endl;

    constexpr float screenWidth = 1200.0f;
    constexpr float screenHeight = 768.0f;

    // ���� ���� �÷��̾� ��ü ��������
    CScene* cur = SceneMgr::Instance()->getScene();
    CObject& pl = cur->getObject(GROUP_TYPE::PLAYER, 0);

    // ī�޶� ����
    CCamera* cam = CameraMgr::Instance()->getMainCamera();

    vec3 plPos = pl.getPosition();
    cam->position = plPos;

    // ���콺 ��ġ�� �������� ī�޶� Ÿ�� ����
    cam->target = vec3(plPos.x + sinf(mousePos.x), plPos.y, plPos.z + cosf(mousePos.y));
}
