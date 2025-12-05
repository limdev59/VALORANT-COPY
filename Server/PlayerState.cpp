#include "PlayerState.h"

PlayerState::PlayerState(PlayerID id)
	: m_PlayerID(id)
	, m_Health(100)
	, m_Position(0.f, 0.f, 0.f)
	, ViewStart(0.f, 0.f, 0.f)
	, ViewEnd(0.f, 0.f, 0.f)
	, m_Velocity(0.f, 0.f, 0.f)
{

}

void PlayerState::ApplyMovementFromClient(const C2S_MovementUpdate& pkt, float serverNow)
{
	// ��Ŷ���� ���� ������ �״�� �ݿ�
	m_Position = pkt.position;
	
}

void PlayerState::ApplyFireFromClient(const C2S_FireAction& pkt)
{
	printf("[PlayerState] Player %u fired at time %.2f\n", m_PlayerID, pkt.clientTime);

	// 
}

void PlayerState::ApplyDamage(int dmg)
{
	if (m_Health <= 0) return;

	m_Health -= (float)dmg;
	if (m_Health < 0) m_Health = 0;

	printf("[PlayerState] Player %u  %d damage. HP: %.1f\n", m_PlayerID, dmg, m_Health);

	if (m_Health <= 0)
	{
		printf("[PlayerState] Player %u DIED.\n", m_PlayerID);
		
		// ��� ������
		// m_Health = 100;
		// m_Position = { 0, 0, 0 }; 
	}
}

PlayerSnapshot PlayerState::ToSnapshot() const
{
	PlayerSnapshot snap;

	snap.id = m_PlayerID;
	snap.position = m_Position;
	snap.viewStart = ViewStart;
	snap.viewEnd = ViewEnd;
	snap.position = m_Position;
	snap.velocity = m_Velocity;
	snap.health = m_Health;
	snap.serverTime = 0.0f;

	return snap;
}

