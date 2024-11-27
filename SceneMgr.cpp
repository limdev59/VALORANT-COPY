#include "pch.h"
#include "SceneMgr.h"
#include "Start_Scene.h"
#include "Stage_1_Scene.h"

SceneMgr::SceneMgr()
    : arrScene{}
    , currentScene(nullptr)
{
}

SceneMgr::~SceneMgr() {
}

void SceneMgr::Init() {
    arrScene[(GLuint)SCENE_TYPE::START] = new Start_Scene;
    arrScene[(GLuint)SCENE_TYPE::STAGE_1] = new Stage_1_Scene;
    arrScene[(GLuint)SCENE_TYPE::START]->setName(L"Start Scene");
    arrScene[(GLuint)SCENE_TYPE::STAGE_1]->setName(L"Stage 1 Scene");
    currentScene = arrScene[(GLuint)SCENE_TYPE::START];
    currentScene->Enter();
}

void SceneMgr::Update() {
    currentScene->Update();
}

void SceneMgr::Render() {
    currentScene->Render();
    
}
void SceneMgr::changeScene(SCENE_TYPE newScene) {
    if (currentScene)
        currentScene->Exit();

    currentScene = arrScene[(GLuint)newScene];
    if (currentScene)
        currentScene->Enter();
}

