#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel 구현 포함
#include "CCore.h"       // 셰이더 ID를 가져오기 위해
#include "define.h"      // MODEL_TYPE::ASCENT 열거형을 사용하기 위해

// 생성자: IModel<Model>을 생성하고 ASCENT 맵을 로드합니다.
Ascent::Ascent()
    : m_pModelDecoration(nullptr)
    , m_pModelFloor(nullptr)
    , m_pModelWall(nullptr)
    , m_pModelProps(nullptr)
{
    m_pModelDecoration  = new IModel<Model>(MODEL_TYPE::ASCENT_DECORATION, GL_TRIANGLES);
    m_pModelFloor       = new IModel<Model>(MODEL_TYPE::ASCENT_FLOOR, GL_TRIANGLES);
    m_pModelWall        = new IModel<Model>(MODEL_TYPE::ASCENT_WALL, GL_TRIANGLES);
    m_pModelProps       = new IModel<Model>(MODEL_TYPE::ASCENT_PROPS, GL_TRIANGLES);
}


    UpdateColliders();
}
bool Ascent::CheckCollisionWithAABB(const glm::vec3& otherCenter, const glm::vec3& otherSize) const {
    glm::vec3 otherHalf = otherSize * 0.5f;
    for (const auto& collider : m_colliders) {
        glm::vec3 halfSize = collider.size * 0.5f;
        if (std::abs(collider.center.x - otherCenter.x) <= (halfSize.x + otherHalf.x) &&
            std::abs(collider.center.y - otherCenter.y) <= (halfSize.y + otherHalf.y) &&
            std::abs(collider.center.z - otherCenter.z) <= (halfSize.z + otherHalf.z)) {
            return true;
        }
    }

    return false;
}

void Ascent::UpdateColliders() {
    m_colliders.clear();

    auto pushFromModel = [this](IModel<Model>* model) {
        if (!model) return;
        glm::vec3 center(0.0f), size(0.0f);
        model->GetModel()->GetWorldAABB(center, size);
        m_colliders.push_back({ center, size });
    };

    pushFromModel(m_pModelFloor);
    pushFromModel(m_pModelWall);
    pushFromModel(m_pModelProps);

    if (!m_colliders.empty()) {
        glm::vec3 minBound(FLT_MAX);
        glm::vec3 maxBound(-FLT_MAX);
        for (const auto& collider : m_colliders) {
            glm::vec3 halfSize = collider.size * 0.5f;
            minBound = glm::min(minBound, collider.center - halfSize);
            maxBound = glm::max(maxBound, collider.center + halfSize);
        }

        glm::vec3 combinedCenter = (minBound + maxBound) * 0.5f;
        glm::vec3 combinedSize = maxBound - minBound;
        setHitbox(combinedCenter - getPosition(), combinedSize, COLLIDER_TYPE::AABB);
    }
}
Ascent::~Ascent() {
    if (m_pModelDecoration) delete m_pModelDecoration;
    if (m_pModelFloor) delete m_pModelFloor;
    if (m_pModelWall) delete m_pModelWall;
    if (m_pModelProps) delete m_pModelProps;
}

void Ascent::Update() {
    // CObject가 가지고 있는 위치, 회전, 크기 정보를 가져옴
    glm::vec3 pos = getPosition();
    glm::vec3 rot = getRotation();
    glm::vec3 scl = getScale();

    // 각 내부 모델들에게 변환 정보 업데이트 요청
    if (m_pModelDecoration) m_pModelDecoration->GetModel()->Update(pos, rot, scl);
    if (m_pModelFloor)      m_pModelFloor->GetModel()->Update(pos, rot, scl);
    if (m_pModelWall)       m_pModelWall->GetModel()->Update(pos, rot, scl);
    if (m_pModelProps)      m_pModelProps->GetModel()->Update(pos, rot, scl);
}

// Render: 정적 모델 셰이더(shaderProgramID)를 활성화하고 모델을 렌더링합니다.
void Ascent::Render() {
    if (m_pModelDecoration) {
        glUseProgram(CCore::Instance()->shaderProgramID);

        m_pModelDecoration->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelFloor->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelWall->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelProps->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}