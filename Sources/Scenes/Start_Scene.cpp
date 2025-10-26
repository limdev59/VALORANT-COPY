#include "pch.h"
#include "Start_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"

#include "Player.h"
#include "Sova.h"

Start_Scene::Start_Scene() {
}

Start_Scene::~Start_Scene() {
}

void Start_Scene::Enter() {
    if (!loaded) {

       // --- [Player 생성 (IModel 리팩토링 적용)] ---
       // Player() 생성자가 'new IModel<AnimModel>("first2")'를 호출합니다.
       CObject* obj = new Player();
        

       CObject* sova = new Sova();
       //sova->setPosition(vec3(0, 0, 0.6));
       //sova->setPosition(vec3(-40, -5, 80));
       //sova->setScale(vec3(1.0f));
        
        addObject(obj, GROUP_TYPE::PLAYER);
        addObject(sova, GROUP_TYPE::DEFAULT);

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