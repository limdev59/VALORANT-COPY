#pragma once
class CCore {
    SINGLE(CCore);
public:
    GLuint  shaderProgramID;
    GLint   width, height;
    void    Update();
    void    Render();
    void    Init();
};
