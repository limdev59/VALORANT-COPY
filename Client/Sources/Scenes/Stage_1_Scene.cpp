#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
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
        player->setPosition(vec3(-2.56633, 0.225f, -4.68781));
        player->setScale(vec3(0.1f));


  
        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-1.49554f, 0.125f,-5.42878),
            glm::vec3(-3.76336f, 0.125f, -6.6219f),  //백사위
            glm::vec3(-0.518212f, 0.125f, -6.9495f),
            glm::vec3(-1.4714f, 0.125f, -9.09569), //ct
            glm::vec3(-4.81171f, -0.1f, -8.39536f),  //백사오른
        };

        std::vector<CObject*> enemies; // 적 객체를 관리하는 컨테이너

        for (const auto& position : enemyPositions) {
            Enemy* enemy = new Enemy();
            enemy->setPosition(position);
            enemy->setScale(glm::vec3(0.1f));

            enemies.push_back(enemy);
            addObject(enemy, GROUP_TYPE::ENEMY);
        }

        // --- 4. 씬에 객체 추가 ---
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
        

        //ApplySnapshot
        if (g_pNetwork)
        {
            // 최신 스냅샷
            const std::vector<PlayerSnapshot>& snapshots = g_pNetwork->GetLastSnapshots();

            for (const auto& snap : snapshots)
            {
                // 내 캐릭터는 스냅샷 적용 제외
                if (snap.id == g_pNetwork->GetMyPlayerID()) continue;

                // 3. 해당 ID의 플레이어가 이미 씬에 있는지 확인
                auto it = m_remotePlayers.find(snap.id);
                Enemy* pEnemy = nullptr;
                
                if (it != m_remotePlayers.end()) {
                    pEnemy = dynamic_cast<Enemy*>(it->second);
                } else {
                    pEnemy = new Enemy();
                    pEnemy->setScale(glm::vec3(0.1f));
                    addObject(pEnemy, GROUP_TYPE::ENEMY);
                    m_remotePlayers[snap.id] = pEnemy;
                    std::cout << "New Player: " << snap.id << std::endl;
                }
                if (pEnemy) {
                    pEnemy->SyncNetworkState(snap);
                }
                
            }
        }
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
    RenderPlayerHUD();
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