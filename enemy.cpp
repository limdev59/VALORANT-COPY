#include "pch.h"
#include "enemy.h"



void Enemy::Update()
{
    if (model) {
        model->Update(position, rotation, scale);
    }
}

void Enemy::TakeDamage(int damage)
{
    hp -= damage; // ��������ŭ ü���� ����
    if (hp < 0) {
        hp = 0; // ü���� 0 ���Ϸ� �������� ����
    }

    // ü���� 0�� �Ǿ��� �� �߰� ���� (��: ��� ó��)
    if (hp == 0) {
        OnDeath();
    }
}

void Enemy::OnDeath()
{
    // ��� �� ó���� ���� (��: ����, �ִϸ��̼� ���)
    // ����� �ܼ��� �α� ���
    position.y = -100;
    std::cout << "Enemy has died!" << std::endl;
}

