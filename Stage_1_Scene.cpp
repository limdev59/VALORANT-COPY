#include "pch.h"
#include "Stage_1_Scene.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "MouseMgr.h"
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
        Model* map_floor_model = new Model(MODEL_TYPE::ASCENT, GL_TRIANGLES);
        map_floor->setModel(map_floor_model);
        map_floor->setScale(vec3(0.1));
        map_floor->setPosition(vec3(0.0f, 0.0f, 0.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(-2.56633, 0.125f, -4.68781));
        player->setScale(vec3(0.1f));
        

        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-1.49554f, 0.125f,-5.42878),
            glm::vec3(-3.76336f, 0.125f, -6.6219f),  //�����
            glm::vec3(-0.518212f, 0.125f, -6.9495f), 
            glm::vec3(-1.4714f, 0.125f, -9.09569), //ct
            glm::vec3(-4.81171f, -0.1f, -8.39536f),  //������
        };

        std::vector<CObject*> enemies; // �� ��ü�� �����ϴ� �����̳�

        for (const auto& position : enemyPositions) {
            Enemy* enemy = new Enemy();
            Model* enemy_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES); // ENEMY Ÿ�� ��
            enemy->setModel(enemy_model);
            enemy->setPosition(position); // ���� ��ġ ����
            enemy->setScale(glm::vec3(0.1f));

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
    cout << player.getPosition().x << ' ' << player.getPosition().y << ' ' << player.getPosition().z << endl;
    CObject& mapFloor = getObject(GROUP_TYPE::DEFAULT, 0);

    Player* playerPtr = dynamic_cast<Player*>(&player);

    if (player.CheckCollision(mapFloor)) {
        //cout << "�浹��" << endl;
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
