#pragma once
enum class CAM_IDX {
    START,
    DEBUG,
    END,
};

class CCamera;
class CameraMgr {
    SINGLE(CameraMgr);
private:
    CCamera* mainCamera;
    CCamera* arrCam[(GLuint)CAM_IDX::END];

public:
    CCamera* getMainCamera();

    void Update();
    void Init();
    void switchMainCam(SCENE_TYPE newScene);
};