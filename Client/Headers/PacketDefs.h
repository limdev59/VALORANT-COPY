#pragma once

#include <cstdint>
#include <string>
#include <vector>

using PlayerID = uint16_t;

struct Vec3 {
	float x;
	float y;
	float z;
};

struct PlayerSnapshot {
	PlayerID id;
	Vec3 position;
	Vec3 rotation;
	Vec3 velocity;
	float health;
	float serverTime{ 0.0f };
	// 추가 필드 필요시 여기에 추가
};

enum class MsgType : uint16_t {
	C2S_LOGIN_REQUEST = 1,  // TCP
	S2C_LOGIN_ACCEPT = 2,  // TCP
	C2S_MOVEMENT_UPDATE = 3,  // UDP
	C2S_FIRE_ACTION = 4,  // UDP
	S2C_SNAPSHOT_STATE = 5,  // UDP
};

struct MsgHeader {
	uint16_t type{ 0 };   // MsgType
	uint16_t reserved{ 0 };
	uint32_t length{ 0 }; // payload 길이(byte)
};

struct C2S_LoginRequest {
	MsgType type		= MsgType::C2S_LOGIN_REQUEST;

	std::string playerName;
	uint16_t    clientUdpPort{ 0 };
};

struct S2C_LoginAccept {
	MsgType		type	= MsgType::S2C_LOGIN_ACCEPT;

	PlayerID	playerId;
};

struct C2S_MovementUpdate {
	MsgType		type = MsgType::C2S_MOVEMENT_UPDATE;

	uint32_t	msgSeq;
	PlayerID	playerId;
	Vec3		position;
	Vec3		rotation;
	Vec3		velocity;
	float		clientTime{ 0.f };
};

struct C2S_FireAction {
	MsgType		type = MsgType::C2S_FIRE_ACTION;

	uint32_t	msgSeq;
	PlayerID	playerId;
	Vec3		fireOrigin;
	Vec3		fireDirection;
	float		clientTime{ 0.f };
};

struct S2C_SnapshotState {
	MsgType		type = MsgType::S2C_SNAPSHOT_STATE;

	std::vector<PlayerSnapshot>	snapshots;
};