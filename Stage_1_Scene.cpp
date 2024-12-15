#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"

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
        Model* map_floor_model = new Model(MODEL_TYPE::PEARL, GL_TRIANGLES);
        map_floor->setModel(map_floor_model);
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(0.0f, 1.0f, 0.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f, 1.0f, 0.0f));
        player->setScale(vec3(0.3f));

        
        std::vector<CObject*> enemies; // 적 객체를 관리하는 컨테이너
        for (int i = 0; i < 5; ++i) {
            Enemy* enemy = new Enemy();
            Model* enemy_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES); // ENEMY 타입 모델
            enemy->setModel(enemy_model);
            enemy->setPosition(glm::vec3(-1.0f + i * 0.5f, 1.0f, 0.0f)); // 적의 위치를 다르게 설정
            enemy->setScale(glm::vec3(0.3f));

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

    CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);
    
    Player* playerPtr = dynamic_cast<Player*>(&player);

   if (player.CheckCollision(mapFloor)) {
       cout << "충돌중" << endl;
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