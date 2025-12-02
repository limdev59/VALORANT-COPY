#pragma once
#include "CObject.h"
#include "IModel.h"      // ���ø� ���� ����

// ���� ����
class Model;
template <typename T>
class IModel;

class Ascent : public CObject {
public:
    struct AABBCollider {
        glm::vec3 min;
        glm::vec3 max;
    };

private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;

    AABBCollider m_floorCollider;
    AABBCollider m_wallCollider;
    AABBCollider m_propsCollider;

    void UpdateColliders();
    AABBCollider BuildColliderFromModel(const IModel<Model>* modelWrapper) const;

public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;

    std::vector<AABBCollider> GetWalkableColliders() const;
    std::vector<AABBCollider> GetWallColliders() const;
}; 