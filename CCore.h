#pragma once
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLint   width, height;

    GLvoid  Reshape(int w, int h);
    GLvoid  Update();
    GLvoid  Render();
    GLvoid  Init();
};
