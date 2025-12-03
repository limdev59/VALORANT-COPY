#pragma once
#include "CObject.h"
#include "IModel.h"

// 전방 선언
class Model;
template <typename T>
class IModel;

class Ascent : public CObject {
private:
    IModel<Model>* m_pModelDecoration;
    IModel<Model>* m_pModelFloor;
    IModel<Model>* m_pModelWall;
    IModel<Model>* m_pModelProps;
    
    bool m_bShowHitbox = false;
    
    std::vector<glm::vec3> m_cachedTriangles;
    bool m_bIsCacheDirty = true;

public:
    Ascent();
    virtual ~Ascent();

    virtual void Update() override;
    virtual void Render() override;
    
    void RenderHitbox();
    void SetShowHitbox(bool show) { m_bShowHitbox = show; }
    bool IsShowHitbox() const { return m_bShowHitbox; }
    
    const std::vector<glm::vec3>& GetAllTriangles();
};