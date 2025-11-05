#include "PlayerState.h"

PlayerState::PlayerState(PlayerID id)
	: m_PlayerID(id)
	, m_Health(100)
	, m_Position(0.f, 0.f, 0.f)
	, m_Rotation(0.f, 0.f, 0.f)
	, m_Velocity(0.f, 0.f, 0.f)
{

}

void PlayerState::ApplyMovementFromClient(const C2S_MovementUpdate& pkt, float serverNow)
{

}

void PlayerState::ApplyFireFromClient(const C2S_FireAction& pkt)
{
}

void PlayerState::ApplyDamage(int dmg)
{
}

PlayerSnapshot PlayerState::ToSnapshot() const
{
	PlayerSnapshot snap;

	snap.id = m_PlayerID;
	snap.position = m_Position;
	snap.rotation = m_Rotation;
	snap.velocity = m_Velocity;
	snap.health = m_Health;
	// TODO: snap.serverTime은 WorldState::Tick()에서 채워줄것

	return snap;
}

