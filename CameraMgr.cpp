#include "pch.h"
#include "CameraMgr.h"
#include "CCamera.h"

#include "Start_Camera.h"
#include "Debug_Camera.h"
#include "AnimModel.h"

CameraMgr::CameraMgr() {
}

CameraMgr::~CameraMgr() {
}

CCamera* CameraMgr::getMainCamera() {
    return mainCamera;
}

void CameraMgr::Update(AnimModel* mainModel) {
    mainCamera->Update();
}

void CameraMgr::Init() {
    arrCam[(GLuint)CAM_IDX::START]  = new Start_Camera;
    arrCam[(GLuint)CAM_IDX::DEBUG]  = new Debug_Camera;
    arrCam[(GLuint)CAM_IDX::START]  ->setName(L"Start Camera");
    arrCam[(GLuint)CAM_IDX::DEBUG]  ->setName(L"Debug Camera");
    mainCamera = arrCam[(GLuint)CAM_IDX::START];
    mainCamera->Init();
    //mainCamera->setAspectRatio(aspectWidth, aspectHeight);
}

void CameraMgr::switchMainCam(SCENE_TYPE newScene)
{
}
