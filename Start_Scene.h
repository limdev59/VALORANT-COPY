#pragma once
#include "CScene.h"

class Start_Scene : public CScene {
public:
    Start_Scene();
    ~Start_Scene();

    virtual void Update();
    virtual void Render();
    virtual void Enter();
    virtual void Exit();
};
