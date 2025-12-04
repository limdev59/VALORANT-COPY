#pragma once

#include "AnimModel.h"
#include "CScene.h"

class ClientNetwork;
class Player;

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

private:
    void InitNetwork();
    void UpdateNetwork();
    void SendPlayerMovement(Player* player);

private:
    ClientNetwork* m_network = nullptr;
};
