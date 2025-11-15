#include "pch.h"
#include "CObject.h"
#include "CCore.h"

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


//bool CObject::CheckCollision(const CObject& other) {
//    if (colliderType == COLLIDER_TYPE::AABB && other.colliderType == COLLIDER_TYPE::AABB) {
//
//        return (abs(hitboxCenter.x - other.hitboxCenter.x) < (hitboxSize.x + other.hitboxSize.x) * 0.5f &&
//            abs(hitboxCenter.y - other.hitboxCenter.y) < (hitboxSize.y + other.hitboxSize.y) * 0.5f &&
//            abs(hitboxCenter.z - other.hitboxCenter.z) < (hitboxSize.z + other.hitboxSize.z) * 0.5f);
//    }
//    return false;
//}


bool CObject::CheckCollision(const CObject& other) {
    // 두 객체 모두 AABB 타입인지 확인
    if (this->colliderType != COLLIDER_TYPE::AABB || other.getColliderType() != COLLIDER_TYPE::AABB) {
        return false;
    }

    // 1. 각 객체의 절대적인 AABB 최소/최대 좌표 계산
    // (position은 객체의 중심, hitboxCenter는 그 중심에서의 오프셋)
    glm::vec3 thisMin = (this->position + this->hitboxCenter) - (this->hitboxSize / 2.0f);
    glm::vec3 thisMax = (this->position + this->hitboxCenter) + (this->hitboxSize / 2.0f);

    glm::vec3 otherMin = (other.getPosition() + other.getHitboxCenter()) - (other.getHitboxSize() / 2.0f);
    glm::vec3 otherMax = (other.getPosition() + other.getHitboxCenter()) + (other.getHitboxSize() / 2.0f);


    // 2. 각 축(x, y, z)에서 겹치는지 확인 (Slab Test)
    // 한 축이라도 겹치지 않으면 충돌하지 않은 것
    if (thisMax.x < otherMin.x || thisMin.x > otherMax.x) return false;
    if (thisMax.y < otherMin.y || thisMin.y > otherMax.y) return false;
    if (thisMax.z < otherMin.z || thisMin.z > otherMax.z) return false;

    // 3. 모든 축에서 겹치면 충돌!
    return true;
}

