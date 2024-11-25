#pragma once
#include "Model.h"

enum class COLLIDER_TYPE {
    NONE,
    AABB,   // Axis-Aligned Bounding Box    축
    OBB,    // Oriented Bounding Box        박스
    SPHERE, // Spherical Bounding Box       구
};

class CObject {
protected:
    glm::vec3       position;
    glm::vec3       rotation;
    glm::vec3       scale;

    glm::vec3       hitboxCenter;
    glm::vec3       hitboxSize;
    COLLIDER_TYPE   colliderType;

    Model* model;

public:
    CObject();

    ~CObject();

    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scl);

    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::vec3 getScale() const;

    void setHitbox(const glm::vec3& center, const glm::vec3& size, COLLIDER_TYPE type);

    glm::vec3 getHitboxCenter() const;
    glm::vec3 getHitboxSize() const;
    COLLIDER_TYPE getColliderType() const;

    void setModel(Model* _model);
    virtual void Update();
    virtual void Render();
    virtual bool CheckCollision(const CObject& other);
};
