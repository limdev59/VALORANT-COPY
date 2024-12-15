#pragma once
#include "CObject.h"
#include "AnimModel.h"

// Player.h
class Player : public CObject {
private:
    float gravity = -9.81f;      // �߷� ��
    vec3 velocity = vec3(0.0f);  // �ӵ�
    bool isOnGround = false;     // �ٴڰ� �浹 ����
    AnimModel* model;
public:
    virtual void Update() override;
    virtual void Render() override;
    void Gravity(bool);  // �߷� ó�� �Լ�
    bool CheckCollisionWithGround(); // �ٴڰ��� �浹 üũ �Լ�
    // ��Ÿ �����...
};

