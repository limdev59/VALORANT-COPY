#pragma once
class Camera {
    SINGLE(Camera);
public:
    glm::vec3       position;
    glm::vec3       target;
    glm::vec3       up;
    float           fov;
    float           aspectRatio;
    float           nearClip;
    float           farClip;
    glm::mat4       GetViewMatrix() const;
    glm::mat4       GetProjectionMatrix() const;
    ProjectionMode  projectionMode;
    void            Init();
    void            Update();
    void            SetAspectRatio(float width, float height);
};


