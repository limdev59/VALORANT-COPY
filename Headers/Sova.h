#pragma once
#include "CObject.h"
#include "AnimModel.h"


class Sova : public CObject {
protected:
    AnimModel* model;

public:
    Sova();
    ~Sova();

    void setModel(AnimModel* _model);
    virtual void Update();
    virtual void Render();
};
