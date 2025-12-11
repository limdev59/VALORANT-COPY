#include "PlayerState.h"

#include <array>
#include <random>

PlayerState::PlayerState(PlayerID id)
        : m_PlayerID(id)
        , m_Health(kMaxHealth)
        , m_IsAlive(true)
        , m_Position(0.f, 0.f, 0.f)
        , m_Velocity(0.f, 0.f, 0.f)
        , m_InputKeys(0)
        , m_IsOnGround(false)
        , m_NextRespawnTime(-1.f)
{

}

void PlayerState::ApplyMovementFromClient(const C2S_MovementUpdate& pkt, float serverNow)
{
	// ��Ŷ���� ���� ������ �״�� �ݿ�
	m_Position = pkt.position;
	m_Rotation = pkt.rotation;
	m_Velocity = pkt.velocity;
	m_InputKeys = pkt.inputKeys;
	m_IsOnGround = pkt.isOnGround;
}

void PlayerState::ApplyFireFromClient(const C2S_FireAction& pkt)
{
	printf("[PlayerState] Player %u fired at time %.2f\n", m_PlayerID, pkt.clientTime);

	// 
}

void PlayerState::ApplyDamage(int dmg, float nowSec)
{
        if (!m_IsAlive) return;

        m_Health -= (float)dmg;
        if (m_Health < 0) m_Health = 0;

	printf("[PlayerState] Player %u  %d damage. HP: %.1f\n", m_PlayerID, dmg, m_Health);

        if (m_Health <= 0)
        {
                m_IsAlive = false;
                printf("[PlayerState] Player %u DIED.\n", m_PlayerID);

                m_NextRespawnTime = nowSec + kRespawnDelay;
        }
}

void PlayerState::Update(float nowSec)
{
        if (!m_IsAlive && m_NextRespawnTime > 0 && nowSec >= m_NextRespawnTime)
        {
                Respawn();
        }
}

void PlayerState::Respawn()
{
        m_IsAlive = true;
        m_Health = kMaxHealth;
        m_Velocity = Vec3(0.f, 0.f, 0.f);
        m_Position = GetRandomSpawnPosition();
        m_NextRespawnTime = -1.f;

        printf("[PlayerState] Player %u respawned with full health.\n", m_PlayerID);
}

Vec3 PlayerState::GetRandomSpawnPosition() const
{
        static const std::array<Vec3, 5> kSpawnPositions = {
                Vec3(-1.49554f, 0.125f, -5.42878f),
                Vec3(-3.76336f, 0.125f, -6.6219f),
                Vec3(-0.518212f, 0.125f, -6.9495f),
                Vec3(-1.4714f, 0.125f, -9.09569f),
                Vec3(-4.81171f, -0.1f, -8.39536f),
        };

        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<size_t> dist(0, kSpawnPositions.size() - 1);

        return kSpawnPositions[dist(rng)];
}

PlayerSnapshot PlayerState::ToSnapshot() const
{
	PlayerSnapshot snap;

	snap.id = m_PlayerID;
	snap.position = m_Position;
	snap.rotation = m_Rotation;
	snap.velocity = m_Velocity;
        snap.inputKeys = m_InputKeys;

        snap.isOnGround = m_IsOnGround;

        snap.health = m_Health;
        snap.isAlive = m_IsAlive;
        snap.serverTime = 0.0f;

	return snap;
}
