#pragma once

#include <cstdint>

constexpr int MAX_NAME_LEN = 32;     
constexpr int MAX_PLAYER_COUNT = 16; 

using PlayerID = uint16_t;

struct Vec3 {
	float x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct PlayerSnapshot {
	PlayerID id;
	Vec3 position;
	Vec3 viewStart;
	Vec3 viewEnd;
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
	MsgType type = MsgType::C2S_LOGIN_REQUEST;

	char playerName[MAX_NAME_LEN];
	uint16_t    clientUdpPort{ 0 };
};

struct S2C_LoginAccept {
	MsgType		type = MsgType::S2C_LOGIN_ACCEPT;

	PlayerID	playerId;
};

struct C2S_MovementUpdate {
	MsgType		type = MsgType::C2S_MOVEMENT_UPDATE;

	uint32_t	msgSeq;
	PlayerID	playerId;
	
	Vec3		position;   // 현재 플레이어 위치
	Vec3		viewStart;  // 시야 시작점 (카메라 위치)
	Vec3		viewEnd;    // 시야 끝점 (카메라 타겟) - 슝민
	
	float		clientTime{ 0.f };
};

struct C2S_FireAction {
	MsgType		type = MsgType::C2S_FIRE_ACTION;

	uint32_t	msgSeq;
	PlayerID	playerId;
	Vec3		fireOrigin;
	Vec3		fireDirection;
	float		clientTime{ 0.f };

	PlayerID    hitPlayerID;
};

struct S2C_SnapshotState {
	MsgType		type = MsgType::S2C_SNAPSHOT_STATE;

	PlayerSnapshot snapshots[MAX_PLAYER_COUNT];
};