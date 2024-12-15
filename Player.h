#pragma once
#include "CObject.h"

// Player.h
class Player : public CObject {
private:
    float gravity = -9.81f;      // �߷� ��
    vec3 velocity = vec3(0.0f);  // �ӵ�
    bool isOnGround = false;     // �ٴڰ� �浹 ����

public:
    virtual void Update() override;
    void Gravity(bool);  // �߷� ó�� �Լ�
    bool CheckCollisionWithGround(); // �ٴڰ��� �浹 üũ �Լ�
    // ��Ÿ �����...
};

