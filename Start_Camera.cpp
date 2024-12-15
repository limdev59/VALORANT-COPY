#include "pch.h"
#include "CCamera.h"
#include "Start_Camera.h"
#include "CScene.h"
#include "SceneMgr.h"
#include "CameraMgr.h"
#include "CObject.h"
#include "MouseMgr.h"

void Start_Camera::Update() {
    // 마우스 정보 업데이트
    MouseMgr::Instance()->Update();

    // 최신 마우스 좌표 가져오기
    auto mousePos = MouseMgr::Instance()->getCursorPos(MOUSE_TYPE::MOVE_HOVER);

    // 마우스 좌표가 잘 바뀌는지 로그로 확인
    std::cout << "Mouse Position: " << mousePos.x << ", " << mousePos.y << std::endl;

    constexpr float screenWidth = 1200.0f;
    constexpr float screenHeight = 768.0f;

    // 현재 씬과 플레이어 객체 가져오기
    CScene* cur = SceneMgr::Instance()->getScene();
    CObject& pl = cur->getObject(GROUP_TYPE::PLAYER, 0);

    // 카메라 설정
    CCamera* cam = CameraMgr::Instance()->getMainCamera();

    vec3 plPos = pl.getPosition();
    cam->position = plPos;

    // 마우스 위치를 기준으로 카메라 타겟 설정
    cam->target = vec3(plPos.x + sinf(mousePos.x), plPos.y, plPos.z + cosf(mousePos.y));
}
