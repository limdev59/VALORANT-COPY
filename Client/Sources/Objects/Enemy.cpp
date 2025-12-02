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
    if (m_pModel) delete m_pModel;
}

void Enemy::Update() {
    if (m_pModel) {
        m_pModel->GetModel()->Update(position, rotation, scale);
    }

    hitboxCenter = glm::vec3(0.0f);
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


