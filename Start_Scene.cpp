#include "pch.h"
#include "Start_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"

#include "Model.h"

#include "Player.h"
#include "Sphere.h"
#include "AxisModel.h"

Start_Scene::Start_Scene() {
}

Start_Scene::~Start_Scene() {
}

void Start_Scene::Enter() {
    if (!loaded) {
        CObject* obj = new Player();
        Model* model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        obj->setModel(model);
        obj->setScale(vec3(0.3f)); 

 

        CObject* obj4 = new AxisModel();
        Model* model4 = new Model(MODEL_TYPE::AXIS_MODEL, GL_LINES);
        
        obj4->setScale(vec3(2.0f));
        obj4->setModel(model4);

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

    vec3 a = MouseMgr::Instance()->getCursorPos(MOUSE_TYPE::MOVE_HOVER);
    float z_ndc = (a.z / 1200) * 2.0f - 1.0f;
    float y_ndc = (a.y / 768) * 2.0f - 1.0f;
    getObject(GROUP_TYPE::DEFAULT, 0).setPosition(vec3(0.0f, y_ndc * 3, z_ndc * 3));

    }

    if (player->CheckCollision(*obj3)) {
        std::cout << "°´Ã¼ 3°ú ºÎµúÄ§!" << std::endl;
    }
    //¿©±â±îÁö
       
    }

    if (player->CheckCollision(*obj3)) {
        std::cout << "°´Ã¼ 3°ú ºÎµúÄ§!" << std::endl;
    }
    //¿©±â±îÁö
       

}

void Start_Scene::Render()
{
    CScene::Render();
}

void Start_Scene::Exit() {
    
}

