#pragma once
#include "CScene.h"
#include <map>
#include <cstdint>

class Stage_1_Scene : public CScene {
public:
    Stage_1_Scene();
    ~Stage_1_Scene();

    virtual void Update();
    virtual void Render();
    virtual void Enter();
    virtual void Exit();
private:
    std::map<uint16_t, class CObject*> m_remotePlayers;
    void RenderPlayerHUD();
};
