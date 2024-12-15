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
        obj->setPosition(vec3(0.0f)); 
        obj->setScale(vec3(0.3f)); 

 

        
        CObject* obj2 = new Sphere();
        Model* model2 = new Model(MODEL_TYPE::SPHERE, GL_TRIANGLES);
        obj2->setModel(model2);
        obj2->setScale(vec3(0.1f));
        
        

        CObject* obj3 = new Sphere();
        Model* model3 = new Model(MODEL_TYPE::SPHERE, GL_TRIANGLES);
        obj3->setModel(model3);
        
        obj->setPosition(vec3(10.0f, 0.0f, 0.0f));

        

        CObject* obj4 = new AxisModel();
        Model* model4 = new Model(MODEL_TYPE::AXIS_MODEL, GL_LINES);
        model4->editVertex([](vector<vec3>& v) {
            v[1] = vec3(1.0f, 0.0f, 0.0f);
            v[3] = vec3(1.0f, 0.0f, 0.0f);
            v[5] = vec3(0.0f, 1.0f, 0.0f);
            v[7] = vec3(0.0f, 1.0f, 0.0f);
            v[9] = vec3(0.0f, 0.0f, 1.0f);
            v[11] = vec3(0.0f, 0.0f, 1.0f);
            });
        obj4->setScale(vec3(2.0f));
        obj4->setModel(model4);

        addObject(obj, GROUP_TYPE::PLAYER);
        addObject(obj2, GROUP_TYPE::DEFAULT);
        addObject(obj3, GROUP_TYPE::DEFAULT);
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
    z_ndc = 1.0f - (a.z / 1200) * 2.0f;
    y_ndc = 1.0f - (a.y / 768) * 2.0f;
    getObject(GROUP_TYPE::DEFAULT, 1).setPosition(vec3(0.0f, y_ndc * 3, z_ndc * 3));


    //충돌 확인용 유안이가 함
    CObject* player = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::PLAYER, 0));
    if (!player) {
        std::cerr << "PLAYER 객체를 CObject*로 변환할 수 없습니다!" << std::endl;
        return;
    }

    CObject* obj2 = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, 0));
    if (!obj2) {
        std::cerr << "DEFAULT 객체를 CObject*로 변환할 수 없습니다!" << std::endl;

    }

    if (player->CheckCollision(*obj2)) {
        std::cout << "갞체 2와부딪침!" << std::endl;
    }

    CObject* obj3 = dynamic_cast<CObject*>(&getObject(GROUP_TYPE::DEFAULT, 1));
    if (!obj3) {
        std::cerr << "DEFAULT 객체를 CObject*로 변환할 수 없습니다!" << std::endl;

    }

    if (player->CheckCollision(*obj3)) {
        std::cout << "객체 3과 부딪침!" << std::endl;
        
    }
    //여기까지
       

}

void Start_Scene::Render()
{
    CScene::Render();
}

void Start_Scene::Exit() {
    
}

