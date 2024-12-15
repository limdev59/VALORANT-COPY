#pragma once
#include "CObject.h"

class Enemy : public CObject
{
private:
    int hp = 100;
    int atk = 30;

public:
    virtual void Update() override;
    void TakeDamage(int damage);
    void OnDeath();
};
