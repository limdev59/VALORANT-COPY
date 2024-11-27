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
        CObject* obj = new Pearl();
        Model* model = new Model(MODEL_TYPE::PEARL, GL_TRIANGLES);
        obj->setModel(model);
        obj->setScale(vec3(0.1));

        addObject(obj, GROUP_TYPE::DEFAULT);
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
