#pragma once

#include "AnimModel.h"
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLuint  shaderProgramID2;
    GLint   width, height;
    
    GLvoid  Reshape(int w, int h);
    GLvoid  Update(AnimModel* mainModel);
    GLvoid  Render();
    GLvoid  Init();
};
