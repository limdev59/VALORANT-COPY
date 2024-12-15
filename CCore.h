#pragma once
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLuint  shaderProgramID2;
    GLint   width, height;

    GLvoid  Reshape(int w, int h);
    GLvoid  Update();
    GLvoid  Render();
    GLvoid  Init();
};
