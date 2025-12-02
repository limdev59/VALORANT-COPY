#pragma once

#include "AnimModel.h"
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLuint  shaderProgramID2;
    GLint   width, height;
    bool    IsHitboxVisible() const { return m_showHitboxes; }
    void    ToggleHitboxVisibility();

    GLvoid  Reshape(int w, int h);
    GLvoid  Update();
    GLvoid  Render();
    GLvoid  Init();

private:
    bool m_showHitboxes = false;
};
