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

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f,1.0f,0.0f));
        player->setScale(vec3(0.3f));

        addObject(map_floor, GROUP_TYPE::DEFAULT);
        addObject(player, GROUP_TYPE::PLAYER);
        loaded = true;
    }

    else {
    }
}

void Stage_1_Scene::Update() {
    CScene::Update();
    if (KeyMgr::Instance()->getKeyState(KEY::NUM_1) == KEY_TYPE::TAP) {
        SceneMgr::Instance()->changeScene(SCENE_TYPE::START);
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}
