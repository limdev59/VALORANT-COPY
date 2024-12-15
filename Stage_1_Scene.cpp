#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"

#include "Model.h"
#include "Player.h"
#include "Pearl.h"
#include "Enemy.h"

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
        map_floor->setPosition(vec3(0.0f, 1.0f, 0.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f, 1.0f, 0.0f));
        player->setScale(vec3(0.3f));

        
        std::vector<CObject*> enemies; // �� ��ü�� �����ϴ� �����̳�
        for (int i = 0; i < 5; ++i) {
            Enemy* enemy = new Enemy();
            Model* enemy_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES); // ENEMY Ÿ�� ��
            enemy->setModel(enemy_model);
            enemy->setPosition(glm::vec3(-1.0f + i * 0.5f, 1.0f, 0.0f)); // ���� ��ġ�� �ٸ��� ����
            enemy->setScale(glm::vec3(0.3f));

            enemies.push_back(enemy); // �����̳ʿ� �߰�
            addObject(enemy, GROUP_TYPE::ENEMY); // ���� ���迡 ���
        }

        addObject(map_floor, GROUP_TYPE::DEFAULT);
        addObject(player, GROUP_TYPE::PLAYER);
        loaded = true;
    }

    else {
    }
}

void Stage_1_Scene::Update() {
    // ���� ������Ʈ �۾�
    CScene::Update();
    CObject& player = getObject(GROUP_TYPE::PLAYER, 0);

    CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);
    
    Player* playerPtr = dynamic_cast<Player*>(&player);

   if (player.CheckCollision(mapFloor)) {
       cout << "�浹��" << endl;
       if (playerPtr) {
           playerPtr->Gravity(false);  // �߷� ����ȵǴ�?
       }
   }
   else {
       
       if (playerPtr) {
           playerPtr->Gravity(true);  // �߷� ����Ǵ�?
       }
   }
}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}