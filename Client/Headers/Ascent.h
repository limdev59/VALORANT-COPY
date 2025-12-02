#pragma once
#include "CObject.h"
    struct Collider {
        glm::vec3 center;
        glm::vec3 size;
    };

    std::vector<Collider> m_colliders;


    const std::vector<Collider>& GetColliders() const { return m_colliders; }
    bool CheckCollisionWithAABB(const glm::vec3& otherCenter, const glm::vec3& otherSize) const;

private:
    void UpdateColliders();
#include "IModel.h"      // 袍敲复 贰欺 器窃

// 傈规 急攫
class Model;
template <typename T>
class IModel;

class Ascent : public CObject {
private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;

public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;
};