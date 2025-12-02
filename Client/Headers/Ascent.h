#pragma once
#include "CObject.h"
#include "IModel.h"      // 

//
class Model;
template <typename T>
class IModel;

struct AABBBounds {
    glm::vec3 min;
    glm::vec3 max;
    bool      isFloor = false;
};

class Ascent : public CObject {
private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;
    std::vector<AABBBounds> m_localColliders;
    std::vector<AABBBounds> m_worldColliders;

    void BuildLocalColliders();
    void UpdateColliderTransforms();
public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;

    const std::vector<AABBBounds>& GetWorldColliders() const { return m_worldColliders; }
};
