#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
// [제거] 씬은 더 이상 Model을 직접 생성하지 않습니다.
// #include "Model.h" 
#include "Player.h"
#include "Ascent.h"
#include "Enemy.h"

Stage_1_Scene::Stage_1_Scene() {
}

Stage_1_Scene::~Stage_1_Scene() {
}

void Stage_1_Scene::Enter() {
    if (!loaded) {


        CObject* map_floor = new Ascent();
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(100.f, 0.0f, 100.f));


        CObject* player = new Player();
        player->setPosition(vec3(-2.56633, 0.125f, -4.68781));
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
        cout << player.getPosition().x << ' ' << player.getPosition().y << ' ' << player.getPosition().z << endl;
        CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);
		mapFloor.setPosition(vec3(player.getPosition().x + 902.f, 0.0f, player.getPosition().z + 1202.f));

    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}