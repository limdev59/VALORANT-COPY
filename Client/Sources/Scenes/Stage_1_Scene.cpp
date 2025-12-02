#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
#include "Player.h"
#include "Ascent.h"
#include "Enemy.h"
#include "CCore.h"
#include "DebugDraw.h"

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
            glm::vec3(-3.76336f, 0.125f, -6.6219f),  //
            glm::vec3(-0.518212f, 0.125f, -6.9495f),
            glm::vec3(-1.4714f, 0.125f, -9.09569), //ct
            glm::vec3(-4.81171f, -0.1f, -8.39536f),  //
        };

        std::vector<CObject*> enemies; //  ü ϴ ̳

        for (const auto& position : enemyPositions) {
            Enemy* enemy = new Enemy();
            enemy->setPosition(position);
            enemy->setScale(glm::vec3(0.1f));

            enemies.push_back(enemy);
            addObject(enemy, GROUP_TYPE::ENEMY);
        }

        // --- 4.  ü ߰ ---
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
            // ֽ 
            const std::vector<PlayerSnapshot>& snapshots = g_pNetwork->GetLastSnapshots();

            for (const auto& snap : snapshots)
            {
                //  ĳʹ   
                if (snap.id == 0) continue;

                // 3. ش ID ÷̾ ̹  ִ Ȯ
                auto it = m_remotePlayers.find(snap.id);

                if (it != m_remotePlayers.end())
                {
                    // ġ ȸ ȭ
                    CObject* pRemoteObj = it->second;
                    pRemoteObj->setPosition(glm::vec3(snap.position.x, snap.position.y, snap.position.z));
                    pRemoteObj->setRotation(glm::vec3(snap.rotation.x, snap.rotation.y, snap.rotation.z));
                }
                else
                {
                    // [] ο ÷̾ ü  (Enemy Ŭ Ȱ)
                   Enemy* newRemotePlayer = new Enemy();
                    newRemotePlayer->setPosition(glm::vec3(snap.position.x, snap.position.y, snap.position.z));
                    newRemotePlayer->setScale(glm::vec3(0.1f)); // ũ 

                    //   ʿ ߰
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

    if (CCore::Instance()->IsHitboxVisible()) {
        auto renderGroup = [&](const std::vector<CObject*>& objects, const glm::vec4& color) {
            for (CObject* obj : objects) {
                if (auto ascent = dynamic_cast<Ascent*>(obj)) {
                    for (const auto& collider : ascent->GetColliders()) {
                        DrawAABB(collider.center, collider.size, color);
                    }
                }
                else {
                    DrawAABB(obj->getPosition() + obj->getHitboxCenter(), obj->getHitboxSize(), color);
                }
            }
        };

        renderGroup(arrObj[(GLuint)GROUP_TYPE::DEFAULT], glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));
        renderGroup(arrObj[(GLuint)GROUP_TYPE::PLAYER], glm::vec4(0.2f, 0.6f, 1.0f, 1.0f));
        renderGroup(arrObj[(GLuint)GROUP_TYPE::ENEMY], glm::vec4(1.0f, 0.8f, 0.2f, 1.0f));
    }
}

void Stage_1_Scene::Exit() {
}