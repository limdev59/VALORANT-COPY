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
    // ���� ������Ʈ �۾�
    CScene::Update();

    // 'player' ��ü�� �����ɴϴ�.
    CObject* player = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::PLAYER, 0));  // ���÷� 0��° �ε���
    if (!player) {
        std::cerr << "PLAYER ��ü�� CObject*�� ��ȯ�� �� �����ϴ�!" << std::endl;
        return;
    }

    CObject* map_floor = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, 0));  // ���÷� 0��° �ε���
    if (!map_floor) {
        std::cerr << "PLAYER ��ü�� CObject*�� ��ȯ�� �� �����ϴ�!" << std::endl;
        return;
    }
    // 'map_floor' ��ü���� ���������� �����ɴϴ�.
    //size_t mapFloorCount = getObjectCount(GROUP_TYPE::DEFAULT);  // ���÷� map_floor�� 'DEFAULT' �׷쿡 ���ԵǾ� �ִٰ� ����
    //for (size_t i = 0; i < mapFloorCount; ++i) {
    //    CObject* mapFloor = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, i));
    //    if (!mapFloor) {
    //        std::cerr << "MAP_FLOOR ��ü�� CObject*�� ��ȯ�� �� �����ϴ�!" << std::endl;
    //        continue;  // �浹�� Ȯ���� �� �����Ƿ� �Ѿ�ϴ�.
    //    }

    //    // �浹 �˻�
    //    if (player->CheckCollision(*mapFloor)) {
    //        std::cout << "�浹��! (map_floor �ε���: " << i << ")" << std::endl;
    //    }
    //}
    if (player->CheckCollision(*map_floor)) {
        std::cout << "�浹��! (map_floor)" << std::endl;
    }

}


void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}