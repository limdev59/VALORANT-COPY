#pragma once
#include "CObject.h"
#include "IModel.h"

class Model;
template <typename T>
class IModel;

class Enemy : public CObject {
protected:
    IModel<Model>* m_pModel;

public:
    Enemy();
    virtual ~Enemy();

    virtual void Update() override;
    virtual void Render() override;

    void TakeDamage(int damage);
    void OnDeath();

private:
    int hp = 100;
    int atk = 30;
};