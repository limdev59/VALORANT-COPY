#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"

#include "Model.h"
#include "Player.h"
#include "Pearl.h"

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
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f, 1.0f, 0.0f));
        player->setScale(vec3(0.3f));

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

    // 'player' 객체를 가져옵니다.
    CObject* player = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::PLAYER, 0));  // 예시로 0번째 인덱스
    if (!player) {
        std::cerr << "PLAYER 객체를 CObject*로 변환할 수 없습니다!" << std::endl;
        return;
    }

    CObject* map_floor = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, 0));  // 예시로 0번째 인덱스
    if (!map_floor) {
        std::cerr << "PLAYER 객체를 CObject*로 변환할 수 없습니다!" << std::endl;
        return;
    }
    // 'map_floor' 객체들을 순차적으로 가져옵니다.
    //size_t mapFloorCount = getObjectCount(GROUP_TYPE::DEFAULT);  // 예시로 map_floor가 'DEFAULT' 그룹에 포함되어 있다고 가정
    //for (size_t i = 0; i < mapFloorCount; ++i) {
    //    CObject* mapFloor = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, i));
    //    if (!mapFloor) {
    //        std::cerr << "MAP_FLOOR 객체를 CObject*로 변환할 수 없습니다!" << std::endl;
    //        continue;  // 충돌을 확인할 수 없으므로 넘어갑니다.
    //    }

    //    // 충돌 검사
    //    if (player->CheckCollision(*mapFloor)) {
    //        std::cout << "충돌함! (map_floor 인덱스: " << i << ")" << std::endl;
    //    }
    //}
    if (player->CheckCollision(*map_floor)) {
        std::cout << "충돌함! (map_floor)" << std::endl;
    }

}


void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}