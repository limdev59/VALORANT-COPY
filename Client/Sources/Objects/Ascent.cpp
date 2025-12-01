#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel
#include "CCore.h"       // 셰이더 ID
#include "define.h"      // MODEL_TYPE::ASCENT 정의
#include "DebugDraw.h"

// : IModel<Model> 를 통해 ASCENT 맵 파트를 로드합니다.
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

// 소멸자: IModel 메모리를 반환합니다.
Ascent::~Ascent() {
    if (m_pModelDecoration) delete m_pModelDecoration;
    if (m_pModelFloor) delete m_pModelFloor;
    if (m_pModelWall) delete m_pModelWall;
    if (m_pModelProps) delete m_pModelProps;
}

void Ascent::Update() {
    // CObject 기본 Transform(위치, 회전, 스케일)
    glm::vec3 pos = getPosition();
    glm::vec3 rot = getRotation();
    glm::vec3 scl = getScale();

    // 각 파트 모델에 Transform 전달
    if (m_pModelDecoration) m_pModelDecoration->GetModel()->Update(pos, rot, scl);
    if (m_pModelFloor)      m_pModelFloor->GetModel()->Update(pos, rot, scl);
    if (m_pModelWall)       m_pModelWall->GetModel()->Update(pos, rot, scl);
    if (m_pModelProps)      m_pModelProps->GetModel()->Update(pos, rot, scl);
}

// Render: 기본 셰이더(shaderProgramID)를 활성화하고 렌더링합니다.
void Ascent::Render() {
    if (m_pModelDecoration) {
        glUseProgram(CCore::Instance()->shaderProgramID);

        m_pModelDecoration->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelFloor->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelWall->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelProps->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}

std::vector<std::pair<glm::vec3, glm::vec3>> Ascent::BuildColliderFromModel(IModel<Model>* model) const {
    std::vector<std::pair<glm::vec3, glm::vec3>> colliders;
    if (!model) return colliders;

    auto bounds = model->GetModel()->GetLocalAABB();
    glm::vec3 scaledMin = bounds.first * getScale();
    glm::vec3 scaledMax = bounds.second * getScale();

    glm::vec3 worldMin = scaledMin + getPosition();
    glm::vec3 worldMax = scaledMax + getPosition();

    colliders.emplace_back(worldMin, worldMax);
    return colliders;
}

std::vector<std::pair<glm::vec3, glm::vec3>> Ascent::GetWorldColliders() const {
    std::vector<std::pair<glm::vec3, glm::vec3>> colliders;

    auto appendColliders = [&](const std::vector<std::pair<glm::vec3, glm::vec3>>& boxes) {
        colliders.insert(colliders.end(), boxes.begin(), boxes.end());
    };

    appendColliders(BuildColliderFromModel(m_pModelFloor));
    appendColliders(BuildColliderFromModel(m_pModelWall));
    appendColliders(BuildColliderFromModel(m_pModelProps));

    return colliders;
}

void Ascent::RenderHitbox(GLuint shaderProgramID) {
    const auto colliders = GetWorldColliders();
    for (const auto& box : colliders) {
        DebugDraw::DrawAABB(shaderProgramID, box.first, box.second, glm::vec3(0.0f, 1.0f, 1.0f));
    }
}
