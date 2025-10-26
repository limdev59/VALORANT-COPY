#include "pch.h"
#include "Enemy.h"
#include "IModel.h"
#include "CCore.h"
#include "define.h"

Enemy::Enemy()
    : CObject() {
    m_pModel = new IModel<Model>(MODEL_TYPE::JETT, GL_TRIANGLES);
}

Enemy::~Enemy() {
    if (m_pModel) delete m_pModel; // 래퍼가 내부의 Model*을 자동 delete 합니다.
}

void Enemy::Update() {
    if (m_pModel) {
        m_pModel->GetModel()->Update(position, rotation, scale);
    }

    // 히트박스
    hitboxCenter = position;
    hitboxSize = scale;
}

void Enemy::Render() {
    if (m_pModel) {
        glUseProgram(CCore::Instance()->shaderProgramID);
        m_pModel->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}

void Enemy::TakeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        OnDeath();
    }
}

void Enemy::OnDeath() {
    std::cout << "Enemy died!" << std::endl;
}


