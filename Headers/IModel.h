#pragma once
#include "pch.h"

class IModel {
public:
    virtual ~IModel() = default; // 가상 소멸자

    // 모든 모델(정적이든, 애니메이션이든)은 렌더링 기능이 있어야 함
    virtual void Render(GLuint shaderProgramID) = 0;

    // 모든 모델은 업데이트 기능이 있어야 함
    virtual void Update(vec3 position, vec3 rotation, vec3 scale) = 0;
};