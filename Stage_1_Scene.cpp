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
        map_floor->setPosition(vec3(39.0f, 5.0f, 80.0f));

        CObject* player = new Player();
        Model* player_model = new Model(MODEL_TYPE::JETT, GL_TRIANGLES);
        player->setModel(player_model);
        player->setPosition(vec3(0.0f, 2.0f, 0.0f));
        player->setScale(vec3(0.1f));


        std::vector<glm::vec3> enemyPositions = {
            glm::vec3(-0.2f, 2.0f, -0.5f),
            glm::vec3(1.5f, 2.0f, -0.5f),
            glm::vec3(-0.2f, 2.0f, 1.8f),
            glm::vec3(1.5f, 2.0f, 0.1f),
            glm::vec3(1.2f, 2.0f, 1.8f),
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
//    // ���콺 Ŭ�� ó��
//    vec3 mousePos = MouseMgr::Instance()->getMouseClick(MOUSE_TYPE::LEFT_CLICK_UP); // ���콺 Ŭ�� ��ġ ��������
//    if (mousePos != vec3(0.0f, 0.0f, 0.0f)) {
//        // ���콺 Ŭ�� ��ġ�� ���� �ִ��� Ȯ��
//        for (CObject* enemy : enemies) {
//            if (enemy && isCollisionWithEnemy(enemy, mousePos)) { // ���� �浹�� �߻��ϸ�
//                Enemy* enemyPtr = dynamic_cast<Enemy*>(enemy);
//                if (enemyPtr) {
//                    enemyPtr->TakeDamage(10);  // ������ ó�� (��: 10)
//                    cout << "������ �������� �־����ϴ�!" << endl;
//                }
//            }
//        }
//    }
//}
//
//bool Stage_1_Scene::isCollisionWithEnemy(CObject* enemy, const vec3& clickPos) {
//    // Ŭ���� ��ġ�� �� ��ü�� ��ġ�� ���� ���� ���� ������ �浹�� �Ǵ�
//    if (enemy) {
//        vec3 enemyPos = enemy->getPosition();
//        float distance = glm::distance(enemyPos, clickPos);
//        return distance < 0.1f;  // ��: 0.1f ���� ���� Ŭ���� ��ġ�� ������ �浹�� ó��
//    }
//    return false;
//}

void Stage_1_Scene::Render()
{
    CScene::Render();
}

void Stage_1_Scene::Exit() {
}
