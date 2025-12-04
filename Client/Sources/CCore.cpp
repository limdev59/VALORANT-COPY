#include "pch.h"
#include "CCore.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "SceneMgr.h"   
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "ClientNetwork.h"
#include "Player.h"

#include "CObject.h"
#include "CCamera.h"
#include "AnimModel.h"

ClientNetwork* g_pNetwork = nullptr;

CCore::CCore() 
{
    if (m_network)
    {
        delete m_network;
        m_network = nullptr;
        g_pNetwork = nullptr;
    }
}

CCore::~CCore() 
{
    if (m_network)
    {
        delete m_network;
        m_network = nullptr;
        g_pNetwork = nullptr;
    }
}

GLvoid CCore::Reshape(int w, int h) {
    MouseMgr::Instance()->setWindowSize(w, h);
    CCore::Instance()->width = w;
    CCore::Instance()->height = (h == 0) ? 1 : h;
    glViewport(0, 0, w, (h == 0) ? 1 : h);
    CameraMgr::Instance()->getMainCamera()->setAspectRatio((float)width, (float)height);
}

GLvoid CCore::Init() {

    TimeMgr::Instance()->Init();
    CameraMgr::Instance()->Init();
    KeyMgr::Instance()->Init();
    SceneMgr::Instance()->Init();
    MouseMgr::Instance()->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

    InitNetwork();
}

GLvoid CCore::Update() {
    UpdateNetwork();
    CameraMgr::Instance()->Update();
    KeyMgr::Instance()->Update();
    SceneMgr::Instance()->Update();
    MouseMgr::Instance()->Update();
    TimeMgr::Instance()->Update();
    glutPostRedisplay();
}

// ·»´õ¸µ ÇÔ¼ö
void CCore::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        glUseProgram(shaderProgramID);
        mat4 projection = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
        mat4 view = CameraMgr::Instance()->getMainCamera()->getViewMatrix();


        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
        glm::vec3 lightColor = glm::vec3(0.9f, 0.83f, 0.83f);
        glm::vec3 cameraPosition = CameraMgr::Instance()->getMainCamera()->position;

        GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
        GLuint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
        GLuint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");

        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPosition));

        GLuint projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
        GLuint viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    }
    TimeMgr::Instance()->Render();
    SceneMgr::Instance()->Render();
}

void CCore::InitNetwork()
{
    if (m_network)
        return;

    m_network = new ClientNetwork();
    g_pNetwork = m_network;

    if (!m_network->ConnectToServer("127.0.0.1", 7777, 9001))
    {
        std::cout << "[CCore] Failed to connect to server." << std::endl;
    }
}

void CCore::UpdateNetwork()
{
    if (!m_network)
        return;

    m_network->PollIncomingPackets();

    CScene* scene = SceneMgr::Instance()->getScene();
    if (!scene)
        return;

    const auto& players = scene->GetObjects(GROUP_TYPE::PLAYER);
    if (players.empty())
        return;

    Player* player = dynamic_cast<Player*>(players[0]);
    if (player)
    {
        SendPlayerMovement(player);
    }
}

void CCore::SendPlayerMovement(Player* player)
{
    if (!g_pNetwork)
        return;

    glm::vec3 velocity = player->GetVelocity();
    bool isMoving = glm::length(glm::vec2(velocity.x, velocity.z)) > 0.0001f || std::abs(velocity.y) > 0.0001f;

    if (!isMoving)
        return;

    C2S_MovementUpdate pkt = player->BuildMovementPacket();
    g_pNetwork->SendMovement(pkt);
}


