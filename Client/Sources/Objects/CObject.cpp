#include "pch.h"
#include "CObject.h"
#include "CCore.h"
#include "DebugDraw.h"

CObject::CObject()
    : position(0.0f), rotation(0.0f), scale(1.0f),
    hitboxCenter(0.0f), hitboxSize(1.0f), colliderType(COLLIDER_TYPE::AABB) {
}

CObject::~CObject() {
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


bool CObject::CheckCollision(const CObject& other) {
    // 현재 AABB만 지원
    if (this->colliderType != COLLIDER_TYPE::AABB || other.getColliderType() != COLLIDER_TYPE::AABB) {
        return false;
    }

    // 1. 양 객체의 AABB 최소/최대 좌표 계산
    glm::vec3 thisMin = (this->position + this->hitboxCenter) - (this->hitboxSize / 2.0f);
    glm::vec3 thisMax = (this->position + this->hitboxCenter) + (this->hitboxSize / 2.0f);

    glm::vec3 otherMin = (other.getPosition() + other.getHitboxCenter()) - (other.getHitboxSize() / 2.0f);
    glm::vec3 otherMax = (other.getPosition() + other.getHitboxCenter()) + (other.getHitboxSize() / 2.0f);


    // 2. 슬랙 테스트로 축별 겹침 여부 확인
    if (thisMax.x < otherMin.x || thisMin.x > otherMax.x) return false;
    if (thisMax.y < otherMin.y || thisMin.y > otherMax.y) return false;
    if (thisMax.z < otherMin.z || thisMin.z > otherMax.z) return false;

    // 3. 여기까지 왔다면 충돌
    return true;
}

void CObject::RenderHitbox(GLuint shaderProgramID) {
    if (shaderProgramID == 0) return;

    glm::vec3 halfSize = hitboxSize / 2.0f;
    glm::vec3 min = (this->position + this->hitboxCenter) - halfSize;
    glm::vec3 max = (this->position + this->hitboxCenter) + halfSize;

    DebugDraw::DrawAABB(shaderProgramID, min, max, glm::vec3(1.0f, 0.0f, 0.0f));
}
