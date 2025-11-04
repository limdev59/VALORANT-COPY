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
        // --- 1. 맵 생성 ---
        // Ascent() 생성자가 IModel<Model>을 통해 스스로 맵 모델을 로드합니다.
        CObject* map_floor = new Ascent();
        // Model* map_floor_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES); [제거]
        // map_floor->setModel(map_floor_model); [제거]
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(0.0f, 0.0f, 0.0f));

        // --- 2. 플레이어 생성 ---
        // Player() 생성자가 IModel<AnimModel>("first2")를 로드합니다.
        CObject* player = new Player();
        // Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES); [제거]
        // player->setModel(player_model); [제거]
        player->setPosition(vec3(-2.56633, 0.125f, -4.68781));
        player->setScale(vec3(0.1f));


        // --- 3. 적 생성 ---
        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-1.49554f, 0.125f,-5.42878),
            glm::vec3(-3.76336f, 0.125f, -6.6219f),  //백사위
            glm::vec3(-0.518212f, 0.125f, -6.9495f),
            glm::vec3(-1.4714f, 0.125f, -9.09569), //ct
            glm::vec3(-4.81171f, -0.1f, -8.39536f),  //백사오른
        };

        std::vector<CObject*> enemies; // 적 객체를 관리하는 컨테이너

        for (const auto& position : enemyPositions) {
            // Enemy() 생성자가 IModel을 통해 스스로 모델을 로드합니다.
            Enemy* enemy = new Enemy();
            // Model* enemy_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES); [제거]
            // enemy->setModel(enemy_model); [제거]
            enemy->setPosition(position); // 적의 위치 설정
            enemy->setScale(glm::vec3(0.1f));

            enemies.push_back(enemy); // 컨테이너에 추가
            addObject(enemy, GROUP_TYPE::ENEMY); // 게임 세계에 등록
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
    // --- [기존 Update 로직 (사용자가 수정한 내용)] ---
    CScene::Update();

    // (getObject 호출은 씬에 객체가 추가된 후에만 안전합니다)
    if (loaded) {
        CObject& player = getObject(GROUP_TYPE::PLAYER, 0);
        cout << player.getPosition().x << ' ' << player.getPosition().y << ' ' << player.getPosition().z << endl;
        CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);

        Player* playerPtr = dynamic_cast<Player*>(&player);
        playerPtr->ApplyGravity(); // [유지] 사용자의 중력 테스트 코드

        // (기존 충돌 체크 로직은 주석 처리)
        //if (player.CheckCollision(mapFloor)) {
        //    if (playerPtr) {
        //        playerPtr->ApplyGravity();  
        //    }
        //}
        //else {
        //    if (playerPtr) {
        //        playerPtr->ApplyGravity();  
        //    }
        //}
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}