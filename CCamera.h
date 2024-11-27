#pragma once
class CCamera {
public:
    CCamera();
    ~CCamera();

    wstring		    strName;

    glm::vec3       position;
    glm::vec3       target;
    glm::vec3       up;

    float           fov;
    float           aspectRatio;
    float           nearClip;
    float           farClip;

    PROJ_TYPE       projType;

    glm::mat4       getViewMatrix() const;
    glm::mat4       getProjectionMatrix() const;
    void            setName(const wstring& _strName) {
        strName = _strName;
    }

    virtual void    Init();
    virtual void    Update();
    virtual void    setAspectRatio(float width, float height);
};


