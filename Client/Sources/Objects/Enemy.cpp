#include "pch.h"
#include "Enemy.h"
#include "IModel.h"
#include "CCore.h"
#include "define.h"

Enemy::Enemy()
    : CObject() {
    m_pModel = new IModel<Model>(MODEL_TYPE::JETT, GL_TRIANGLES);
    setHitbox(glm::vec3(0.0f, 0.6f, 0.0f), glm::vec3(0.6f, 1.6f, 0.6f), COLLIDER_TYPE::AABB);
}

Enemy::~Enemy() {
    if (m_pModel) delete m_pModel;
}

void Enemy::Update() {
    if (m_pModel) {
        m_pModel->GetModel()->Update(position, rotation, scale);
    }

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


