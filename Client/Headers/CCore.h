#pragma once

#include "AnimModel.h"
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLuint  shaderProgramID2;
    GLuint  debugShaderID;
    GLint   width, height;
    bool    showHitboxes = false;

    GLvoid  Reshape(int w, int h);
    GLvoid  Update();
    GLvoid  Render();
    GLvoid  Init();
};
