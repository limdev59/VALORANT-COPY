#pragma once
#include "CScene.h"

class Stage_1_Scene : public CScene {
public:
    Stage_1_Scene();
    ~Stage_1_Scene();

    virtual void Update();
    virtual void Render();
    virtual void Enter();
    virtual void Exit();
};
