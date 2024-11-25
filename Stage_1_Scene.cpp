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
    CObject* obj = new Player();
    obj->setPosition(vec3(0.5f, 0.0f, 0.0f));
    Model* model = new Model(MODEL_TYPE::CUBE, GL_TRIANGLES);
    obj->setModel(model);

    AddObject(obj, GROUP_TYPE::PLAYER);
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
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        for (size_t j = 0; j < arrObj[i].size(); ++j) {
            delete arrObj[i][j];
        }
        arrObj[i].clear();
    }
}
