#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel ���� ����
#include "CCore.h"       // ���̴� ID�� �������� ����
#include "define.h"      // MODEL_TYPE::ASCENT �������� ����ϱ� ����

// ������: IModel<Model>�� �����ϰ� ASCENT ���� �ε��մϴ�.
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

// �Ҹ���: IModel ���۸� �����ϰ� �����մϴ�.
Ascent::~Ascent() {
    if (m_pModelDecoration) delete m_pModelDecoration;
    if (m_pModelFloor) delete m_pModelFloor;
    if (m_pModelWall) delete m_pModelWall;
    if (m_pModelProps) delete m_pModelProps;
}

void Ascent::Update() {
    // CObject�� ������ �ִ� ��ġ, ȸ��, ũ�� ������ ������
    glm::vec3 pos = getPosition();
    glm::vec3 rot = getRotation();
    glm::vec3 scl = getScale();

    // �� ���� �𵨵鿡�� ��ȯ ���� ������Ʈ ��û
    if (m_pModelDecoration) m_pModelDecoration->GetModel()->Update(pos, rot, scl);
    if (m_pModelFloor)      m_pModelFloor->GetModel()->Update(pos, rot, scl);
    if (m_pModelWall)       m_pModelWall->GetModel()->Update(pos, rot, scl);
    if (m_pModelProps)      m_pModelProps->GetModel()->Update(pos, rot, scl);

    UpdateColliders();
}

// Render: ���� �� ���̴�(shaderProgramID)�� Ȱ��ȭ�ϰ� ���� �������մϴ�.
void Ascent::Render() {
    if (m_pModelDecoration) {
        glUseProgram(CCore::Instance()->shaderProgramID);

        m_pModelDecoration->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelFloor->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelWall->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelProps->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}

void Ascent::UpdateColliders() {
    m_floorCollider = BuildColliderFromModel(m_pModelFloor);
    m_wallCollider = BuildColliderFromModel(m_pModelWall);
    m_propsCollider = BuildColliderFromModel(m_pModelProps);
}

Ascent::AABBCollider Ascent::BuildColliderFromModel(const IModel<Model>* modelWrapper) const {
    AABBCollider collider{ {0.0f}, {0.0f} };
    if (!modelWrapper) return collider;

    const Model* model = modelWrapper->GetModel();
    if (!model || !model->HasBounds()) return collider;

    const glm::vec3 localMin = model->GetLocalMin();
    const glm::vec3 localMax = model->GetLocalMax();

    const glm::vec3 scl = getScale();
    const glm::vec3 pos = getPosition();

    glm::vec3 scaledMin = localMin * scl;
    glm::vec3 scaledMax = localMax * scl;

    collider.min = glm::min(scaledMin, scaledMax) + pos;
    collider.max = glm::max(scaledMin, scaledMax) + pos;

    return collider;
}

std::vector<Ascent::AABBCollider> Ascent::GetWalkableColliders() const {
    std::vector<AABBCollider> result;
    if (m_pModelFloor && m_pModelFloor->GetModel()->HasBounds()) {
        result.push_back(m_floorCollider);
    }
    if (m_pModelProps && m_pModelProps->GetModel()->HasBounds()) {
        result.push_back(m_propsCollider);
    }
    return result;
}

std::vector<Ascent::AABBCollider> Ascent::GetWallColliders() const {
    std::vector<AABBCollider> result;
    if (m_pModelWall && m_pModelWall->GetModel()->HasBounds()) {
        result.push_back(m_wallCollider);
    }
    if (m_pModelProps && m_pModelProps->GetModel()->HasBounds()) {
        result.push_back(m_propsCollider);
    }
    return result;
}