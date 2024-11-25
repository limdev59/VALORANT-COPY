#include "pch.h"
#include "Stage_1_Scene.h"
#include "Player.h"
#include "Model.h"
#include "KeyMgr.h"
#include "SceneMgr.h"

Stage_1_Scene::Stage_1_Scene() {
}

Stage_1_Scene::~Stage_1_Scene() {
}

void Stage_1_Scene::Enter() {
    if (!loaded) {
        CObject* obj = new Player();
        Model* model = new Model(MODEL_TYPE::CUBE, GL_TRIANGLES);
        obj->setPosition(vec3(0.5f, 0.0f, 0.0f));
        obj->setModel(model);

        AddObject(obj, GROUP_TYPE::PLAYER);
        loaded = true;
    }
    else {
    }
}

void Stage_1_Scene::Update() {
    CScene::Update();
    if (KeyMgr::Instance()->getKeyState(KEY::NUM_2) == KEY_TYPE::TAP) {
        SceneMgr::Instance()->changeScene(SCENE_TYPE::START);
    }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}
