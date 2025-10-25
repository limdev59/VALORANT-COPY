#include "pch.h"
#include "CObject.h"

CObject::CObject()
    : position(0.0f), rotation(0.0f), scale(1.0f),
    hitboxCenter(0.0f), hitboxSize(1.0f), colliderType(COLLIDER_TYPE::AABB), model(nullptr) {}
CObject::~CObject() {
    if (model) delete model;
}

void CObject::setPosition(const glm::vec3& pos) { position = pos; }
void CObject::setRotation(const glm::vec3& rot) { rotation = rot; }
void CObject::setScale(const glm::vec3& scl) { scale = scl; }
glm::vec3 CObject::getPosition() const { return position; }
glm::vec3 CObject::getRotation() const { return rotation; }
glm::vec3 CObject::getScale() const { return scale; }

void CObject::setHitbox(const glm::vec3& center, const glm::vec3& size, COLLIDER_TYPE type) {
    hitboxCenter = center;
    hitboxSize = size;
    colliderType = type;
}
glm::vec3 CObject::getHitboxCenter() const { return hitboxCenter; }
glm::vec3 CObject::getHitboxSize() const { return hitboxSize; }
COLLIDER_TYPE CObject::getColliderType() const { return colliderType; }
void CObject::setModel(Model* _model) {
    model = _model;
}
void CObject::Update() {
    if (model) {
        model->Update(position, rotation, scale);
    }
    hitboxCenter = position;
    hitboxSize = scale;
}
void CObject::Render() {
    if (model) {
        model->Render(m_shaderProgramID);
    }
}
bool CObject::CheckCollision(const CObject& other) {
    if (colliderType == COLLIDER_TYPE::AABB && other.colliderType == COLLIDER_TYPE::AABB) {
        
        return (abs(hitboxCenter.x - other.hitboxCenter.x) < (hitboxSize.x + other.hitboxSize.x) * 0.5f &&
            abs(hitboxCenter.y - other.hitboxCenter.y) < (hitboxSize.y + other.hitboxSize.y) * 0.5f &&
            abs(hitboxCenter.z - other.hitboxCenter.z) < (hitboxSize.z + other.hitboxSize.z) * 0.5f);
    }
    return false;
}

