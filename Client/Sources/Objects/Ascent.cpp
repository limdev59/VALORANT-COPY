#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel
#include "CCore.h"       // ̴ ID
#include "define.h"      // MODEL_TYPE::ASCENT  ϱ

// : IModel<Model> ϰ ASCENT  εմϴ.
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

    BuildLocalColliders();
}

// Ҹ: IModel ۸ ϰ մϴ.
Ascent::~Ascent() {
    if (m_pModelDecoration) delete m_pModelDecoration;
    if (m_pModelFloor) delete m_pModelFloor;
    if (m_pModelWall) delete m_pModelWall;
    if (m_pModelProps) delete m_pModelProps;
}



void Ascent::BuildLocalColliders() {
    m_localColliders.clear();

    auto addColliders = [&](IModel<Model>* model, bool isFloor) {
        if (!model) return;

        for (const auto& bounds : model->GetModel()->CalculateSubMeshAABBs()) {
            m_localColliders.push_back({ bounds.first, bounds.second, isFloor });
        }
    };

    addColliders(m_pModelFloor, true);
    addColliders(m_pModelWall, false);
    addColliders(m_pModelProps, false);

    UpdateColliderTransforms();
}

void Ascent::UpdateColliderTransforms() {
    glm::vec3 pos = getPosition();
    glm::vec3 scl = getScale();

    m_worldColliders.clear();
    for (const auto& local : m_localColliders) {
        glm::vec3 scaledMin = local.min * scl;
        glm::vec3 scaledMax = local.max * scl;
        glm::vec3 worldMin = scaledMin + pos;
        glm::vec3 worldMax = scaledMax + pos;

        AABBBounds worldBounds;
        worldBounds.min = glm::min(worldMin, worldMax);
        worldBounds.max = glm::max(worldMin, worldMax);
        worldBounds.isFloor = local.isFloor;

        m_worldColliders.push_back(worldBounds);
    }
}

void Ascent::Update() {
    // CObject  ִ ġ, ȸ, ũ
    glm::vec3 pos = getPosition();
    glm::vec3 rot = getRotation();
    glm::vec3 scl = getScale();

    //   𵨵鿡 ȯ  Ʈ û
    if (m_pModelDecoration) m_pModelDecoration->GetModel()->Update(pos, rot, scl);
    if (m_pModelFloor)      m_pModelFloor->GetModel()->Update(pos, rot, scl);
    if (m_pModelWall)       m_pModelWall->GetModel()->Update(pos, rot, scl);
    if (m_pModelProps)      m_pModelProps->GetModel()->Update(pos, rot, scl);

    UpdateColliderTransforms();
}

// Render:   ̴(shaderProgramID) Ȱȭϰ  մϴ.
void Ascent::Render() {
    if (m_pModelDecoration) {
        glUseProgram(CCore::Instance()->shaderProgramID);

        m_pModelDecoration->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelFloor->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelWall->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelProps->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}
