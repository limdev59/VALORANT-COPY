#pragma once
#include "CObject.h"
#include "IModel.h"      // ø

//
class Model;
template <typename T>
class IModel;

class Ascent : public CObject {
private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;

    std::vector<std::pair<glm::vec3, glm::vec3>> BuildColliderFromModel(IModel<Model>* model) const;

public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;
    virtual void RenderHitbox(GLuint shaderProgramID) override;

    std::vector<std::pair<glm::vec3, glm::vec3>> GetWorldColliders() const;
};
