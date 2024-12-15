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
    hp -= damage; // 데미지만큼 체력을 감소
    if (hp < 0) {
        hp = 0; // 체력은 0 이하로 내려가지 않음
    }

    // 체력이 0이 되었을 때 추가 동작 (예: 사망 처리)
    if (hp == 0) {
        OnDeath();
    }
}

void Enemy::OnDeath()
{
    // 사망 시 처리할 로직 (예: 제거, 애니메이션 재생)
    // 현재는 단순히 로그 출력
    position.y = -100;
    std::cout << "Enemy has died!" << std::endl;
}

