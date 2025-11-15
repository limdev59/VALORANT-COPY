#pragma once
#include "CObject.h"
#include "IModel.h"      // 템플릿 래퍼 포함

// 전방 선언
class Model;
template <typename T>
class IModel;

class Ascent : public CObject {
private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;

public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;
};