#include "pch.h"
#include "Start_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"

#include "Model.h"

#include "Player.h"
#include "Sphere.h"
#include "Sova.h"

Start_Scene::Start_Scene() {
}

Start_Scene::~Start_Scene() {
}

void Start_Scene::Enter() {
    if (!loaded) {
        CObject* obj = new Player();
        Model* model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        obj->setModel(model);
        obj->setScale(vec3(0.1f));
        obj->setPosition(vec3(0.0f, 0.0f, 0.0f));
        obj->setRotation(vec3(0.0f));

        CObject* obj4 = new Sova();
        Model* model4 = new Model(MODEL_TYPE::ASCENT, GL_LINES);
        obj4->setModel(model4);
        obj4->setPosition(vec3(-40,-5, 80));
        obj4->setScale(vec3(2.0f));
        obj4->setRotation(vec3(0.0f));
        

        addObject(obj, GROUP_TYPE::PLAYER);
        addObject(obj4, GROUP_TYPE::DEFAULT);
        loaded = true;
    }
    else {
        
    }
}

void Start_Scene::Update() {
    CScene::Update();
    if (KeyMgr::Instance()->getKeyState(KEY::NUM_2) == KEY_TYPE::TAP) {
        SceneMgr::Instance()->changeScene(SCENE_TYPE::STAGE_1);
    }

}

void Start_Scene::Render()
{
    CScene::Render();
}

void Start_Scene::Exit() {
    
}

