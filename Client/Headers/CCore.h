#pragma once

#include "AnimModel.h"
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLuint  shaderProgramID2;
    GLint   width, height;
    bool    m_drawHitboxes = false;

    GLvoid  Reshape(int w, int h);
    GLvoid  Update();
    GLvoid  Render();
    GLvoid  Init();

    bool    IsHitboxDebugEnabled() const { return m_drawHitboxes; }
    void    ToggleHitboxDebug() { m_drawHitboxes = !m_drawHitboxes; }
};
