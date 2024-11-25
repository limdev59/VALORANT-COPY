#include "pch.h"
#include "Start_Scene.h"
#include "Player.h"
#include "Model.h"
#include "KeyMgr.h"
#include "SceneMgr.h"

Start_Scene::Start_Scene() {
}

Start_Scene::~Start_Scene() {
}

void Start_Scene::Enter() {
    if (!loaded) {
        CObject* obj = new Player();
        Model* model = new Model(MODEL_TYPE::CUBE, GL_TRIANGLES);
        obj->setModel(model);

        AddObject(obj, GROUP_TYPE::PLAYER);
        loaded = true;
    }
    else {
        
    }
}

void Start_Scene::Update() {
    CScene::Update();
    if (KeyMgr::Instance()->getKeyState(KEY::NUM_1) == KEY_TYPE::TAP) {
        SceneMgr::Instance()->changeScene(SCENE_TYPE::STAGE_1);
    }
}

void Start_Scene::Render()
{
    CScene::Render();
}

void Start_Scene::Exit() {
}
