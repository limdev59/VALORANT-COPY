#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
#include "Model.h"
#include "Player.h"
#include "Pearl.h"
#include "Enemy.h"

Stage_1_Scene::Stage_1_Scene() {
}

Stage_1_Scene::~Stage_1_Scene() {
}

void Stage_1_Scene::Enter() {
    if (!loaded) {
        CObject* map_floor = new Pearl();
        Model* map_floor_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES);
        map_floor->setModel(map_floor_model);
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(0.0f, 0.0f, 0.0f));
        map_floor->setRotation(vec3(0.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(-2.56633, 0.125f, -4.68781));
        player->setScale(vec3(0.1f));
        player->setRotation(vec3(0.0f));
        

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
            Model* enemy_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES); // ENEMY 타입 모델
            enemy->setModel(enemy_model);
            enemy->setPosition(position); // 적의 위치 설정
            enemy->setScale(glm::vec3(0.1f));
            enemy->setRotation(vec3(0.0f));

            enemies.push_back(enemy); // 컨테이너에 추가
            addObject(enemy, GROUP_TYPE::ENEMY); // 게임 세계에 등록
        }

        addObject(map_floor, GROUP_TYPE::DEFAULT);
        addObject(player, GROUP_TYPE::PLAYER);
        loaded = true;
    }

    else {
    }
}

void Stage_1_Scene::Update() {
    // 기존 업데이트 작업
    CScene::Update();
    CObject& player = getObject(GROUP_TYPE::PLAYER, 0);
    cout << player.getPosition().x << ' ' << player.getPosition().y << ' ' << player.getPosition().z << endl;
    CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);

    Player* playerPtr = dynamic_cast<Player*>(&player);

    if (player.CheckCollision(mapFloor)) {
        //cout << "충돌중" << endl;
        if (playerPtr) {
            playerPtr->Gravity(false);  // 중력 적용안되니?
        }
    }
    else {
        if (playerPtr) {
            playerPtr->Gravity(true);  // 중력 적용되니?
        }
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}
