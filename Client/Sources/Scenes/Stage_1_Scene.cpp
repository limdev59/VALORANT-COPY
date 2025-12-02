#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
#include "CCore.h"
#include "Player.h"
#include "Ascent.h"
#include "Enemy.h"

#include "ClientNetwork.h"
#include "PacketDefs.h"

extern ClientNetwork* g_pNetwork;

Stage_1_Scene::Stage_1_Scene() {
}

Stage_1_Scene::~Stage_1_Scene() {
}

void Stage_1_Scene::Enter() {
    if (!loaded) {


        CObject* map_floor = new Ascent();
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(0.0f, 0.0f, 0.0f));


        CObject* player = new Player();
        player->setPosition(vec3(-2.56633, 0.125f, -4.68781));
        player->setScale(vec3(0.1f));


  
        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-1.49554f, 0.125f,-5.42878),
            glm::vec3(-3.76336f, 0.125f, -6.6219f),  //�����
            glm::vec3(-0.518212f, 0.125f, -6.9495f),
            glm::vec3(-1.4714f, 0.125f, -9.09569), //ct
            glm::vec3(-4.81171f, -0.1f, -8.39536f),  //������
        };

        std::vector<CObject*> enemies; // �� ��ü�� �����ϴ� �����̳�

        for (const auto& position : enemyPositions) {
            Enemy* enemy = new Enemy();
            enemy->setPosition(position);
            enemy->setScale(glm::vec3(0.1f));

            enemies.push_back(enemy);
            addObject(enemy, GROUP_TYPE::ENEMY);
        }

        // --- 4. ���� ��ü �߰� ---
        addObject(map_floor, GROUP_TYPE::DEFAULT);
        addObject(player, GROUP_TYPE::PLAYER);
        loaded = true;
    }
    else {
    }
}

void Stage_1_Scene::Update() {
    CScene::Update();

    if (loaded) {
        CObject& player = getObject(GROUP_TYPE::PLAYER, 0);

        //CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);
        //mapFloor.setPosition(vec3(0.0f, 0.0f, 0.0f));

        //ApplySnapshot
        if (g_pNetwork)
        {
            // �ֽ� ������
            const std::vector<PlayerSnapshot>& snapshots = g_pNetwork->GetLastSnapshots();

            for (const auto& snap : snapshots)
            {
                // �� ĳ���ʹ� ������ ���� ����
                if (snap.id == 0) continue;

                // 3. �ش� ID�� �÷��̾ �̹� ���� �ִ��� Ȯ��
                auto it = m_remotePlayers.find(snap.id);

                if (it != m_remotePlayers.end())
                {
                    // ��ġ ȸ�� ����ȭ
                    CObject* pRemoteObj = it->second;
                    pRemoteObj->setPosition(glm::vec3(snap.position.x, snap.position.y, snap.position.z));
                    pRemoteObj->setRotation(glm::vec3(snap.rotation.x, snap.rotation.y, snap.rotation.z));
                }
                else
                {
                    // [����] ���ο� �÷��̾� ��ü ���� (Enemy Ŭ���� ��Ȱ��)
                   Enemy* newRemotePlayer = new Enemy();
                    newRemotePlayer->setPosition(glm::vec3(snap.position.x, snap.position.y, snap.position.z));
                    newRemotePlayer->setScale(glm::vec3(0.1f)); // ũ�� ����

                    // ���� ���� �ʿ� �߰�
                    addObject(newRemotePlayer, GROUP_TYPE::ENEMY);
                    m_remotePlayers[snap.id] = newRemotePlayer;

                    std::cout << "[ApplySnapshot] New Player Joined: ID " << snap.id << std::endl;
                }
            }
        }
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
    if (CCore::Instance()->IsHitboxDebugEnabled()) {
        RenderHitboxes();
    }
    RenderPlayerHUD();
}

void Stage_1_Scene::RenderHitboxes()
{
    auto drawAABB = [](const glm::vec3& min, const glm::vec3& max, const glm::vec3& color)
    {
        glUseProgram(0);
        glColor3f(color.r, color.g, color.b);
        glLineWidth(2.0f);

        glm::vec3 p1 = glm::vec3(min.x, min.y, min.z);
        glm::vec3 p2 = glm::vec3(max.x, min.y, min.z);
        glm::vec3 p3 = glm::vec3(max.x, min.y, max.z);
        glm::vec3 p4 = glm::vec3(min.x, min.y, max.z);
        glm::vec3 p5 = glm::vec3(min.x, max.y, min.z);
        glm::vec3 p6 = glm::vec3(max.x, max.y, min.z);
        glm::vec3 p7 = glm::vec3(max.x, max.y, max.z);
        glm::vec3 p8 = glm::vec3(min.x, max.y, max.z);

        glBegin(GL_LINES);
        // bottom
        glVertex3fv(glm::value_ptr(p1)); glVertex3fv(glm::value_ptr(p2));
        glVertex3fv(glm::value_ptr(p2)); glVertex3fv(glm::value_ptr(p3));
        glVertex3fv(glm::value_ptr(p3)); glVertex3fv(glm::value_ptr(p4));
        glVertex3fv(glm::value_ptr(p4)); glVertex3fv(glm::value_ptr(p1));

        // top
        glVertex3fv(glm::value_ptr(p5)); glVertex3fv(glm::value_ptr(p6));
        glVertex3fv(glm::value_ptr(p6)); glVertex3fv(glm::value_ptr(p7));
        glVertex3fv(glm::value_ptr(p7)); glVertex3fv(glm::value_ptr(p8));
        glVertex3fv(glm::value_ptr(p8)); glVertex3fv(glm::value_ptr(p5));

        // sides
        glVertex3fv(glm::value_ptr(p1)); glVertex3fv(glm::value_ptr(p5));
        glVertex3fv(glm::value_ptr(p2)); glVertex3fv(glm::value_ptr(p6));
        glVertex3fv(glm::value_ptr(p3)); glVertex3fv(glm::value_ptr(p7));
        glVertex3fv(glm::value_ptr(p4)); glVertex3fv(glm::value_ptr(p8));
        glEnd();

        glLineWidth(1.0f);
    };

    if (!arrObj[(GLuint)GROUP_TYPE::PLAYER].empty())
    {
        Player* player = dynamic_cast<Player*>(arrObj[(GLuint)GROUP_TYPE::PLAYER][0]);
        if (player)
        {
            auto playerAABB = player->GetWorldAABB();
            drawAABB(playerAABB.first, playerAABB.second, glm::vec3(0.1f, 0.8f, 0.1f));
        }
    }

    if (!arrObj[(GLuint)GROUP_TYPE::DEFAULT].empty())
    {
        Ascent* map = dynamic_cast<Ascent*>(arrObj[(GLuint)GROUP_TYPE::DEFAULT][0]);
        if (map)
        {
            const auto walkables = map->GetWalkableColliders();
            const auto blockers = map->GetWallColliders();

            if (!walkables.empty())
            {
                drawAABB(walkables[0].min, walkables[0].max, glm::vec3(0.2f, 0.4f, 1.0f));
                if (walkables.size() > 1)
                {
                    drawAABB(walkables[1].min, walkables[1].max, glm::vec3(0.6f, 0.2f, 1.0f));
                }
            }

            if (!blockers.empty())
            {
                drawAABB(blockers[0].min, blockers[0].max, glm::vec3(1.0f, 0.2f, 0.2f));
            }
        }
    }
}

void Stage_1_Scene::Exit() {
}

void Stage_1_Scene::RenderPlayerHUD()
{
    if (arrObj[(GLuint)GROUP_TYPE::PLAYER].empty())
        return;

    Player* player = dynamic_cast<Player*>(arrObj[(GLuint)GROUP_TYPE::PLAYER][0]);
    if (!player)
        return;

    const std::string hpText = "HP: " + std::to_string(player->GetHealth());

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(20.0f, WINDOW_HEIGHT - 40.0f);
    for (char c : hpText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}