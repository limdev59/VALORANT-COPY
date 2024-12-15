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
        map_floor->setPosition(vec3(39.0f, 5.0f, 80.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f, 2.0f, 0.0f));
        player->setScale(vec3(0.1f));


        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-0.2f, 2.0f, -0.5f),
            glm::vec3(1.5f, 2.0f, -0.5f),
            glm::vec3(-0.2f, 2.0f, 1.8f),
            glm::vec3(1.5f, 2.0f, 0.1f),
            glm::vec3(1.2f, 2.0f, 1.8f),
        };

        std::vector<CObject*> enemies; // 적 객체를 관리하는 컨테이너

        for (const auto& position : enemyPositions) {
            Enemy* enemy = new Enemy();
            Model* enemy_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES); // ENEMY 타입 모델
            enemy->setModel(enemy_model);
            enemy->setPosition(position); // 적의 위치 설정
            enemy->setScale(glm::vec3(0.1f));

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
//    // 마우스 클릭 처리
//    vec3 mousePos = MouseMgr::Instance()->getMouseClick(MOUSE_TYPE::LEFT_CLICK_UP); // 마우스 클릭 위치 가져오기
//    if (mousePos != vec3(0.0f, 0.0f, 0.0f)) {
//        // 마우스 클릭 위치에 적이 있는지 확인
//        for (CObject* enemy : enemies) {
//            if (enemy && isCollisionWithEnemy(enemy, mousePos)) { // 적과 충돌이 발생하면
//                Enemy* enemyPtr = dynamic_cast<Enemy*>(enemy);
//                if (enemyPtr) {
//                    enemyPtr->TakeDamage(10);  // 데미지 처리 (예: 10)
//                    cout << "적에게 데미지를 주었습니다!" << endl;
//                }
//            }
//        }
//    }
//}
//
//bool Stage_1_Scene::isCollisionWithEnemy(CObject* enemy, const vec3& clickPos) {
//    // 클릭된 위치와 적 객체의 위치가 일정 범위 내에 있으면 충돌로 판단
//    if (enemy) {
//        vec3 enemyPos = enemy->getPosition();
//        float distance = glm::distance(enemyPos, clickPos);
//        return distance < 0.1f;  // 예: 0.1f 범위 내에 클릭된 위치가 있으면 충돌로 처리
//    }
//    return false;
//}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}
