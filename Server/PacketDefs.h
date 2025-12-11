#pragma once

#include <cstdint>

constexpr int MAX_NAME_LEN = 32;
constexpr int MAX_PLAYER_COUNT = 16;

using PlayerID = uint16_t;

enum InputKey : uint8_t {
        KEY_NONE = 0,
        KEY_W = 1 << 0,
        KEY_A = 1 << 1,
        KEY_S = 1 << 2,
        KEY_D = 1 << 3,
};

struct Vec3 {
        float x, y, z;
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
};

struct PlayerSnapshot {
        PlayerID id;
        Vec3 position;
        Vec3 rotation;
        Vec3 velocity;
        uint8_t inputKeys;
        bool isOnGround;
        float health;
        bool isAlive;
        float serverTime{ 0.0f };
        // 추가 필드 필요시 여기에 추가
};

enum class MsgType : uint16_t {
        C2S_LOGIN_REQUEST = 1,  // TCP
        S2C_LOGIN_ACCEPT = 2,  // TCP
        C2S_MOVEMENT_UPDATE = 3,  // UDP
        C2S_FIRE_ACTION = 4,  // UDP
        S2C_SNAPSHOT_STATE = 5,  // UDP
        S2C_FIRE_EVENT = 6      // UDP
};

struct MsgHeader {
        uint16_t type{ 0 };   // MsgType
        uint16_t reserved{ 0 };
        uint32_t length{ 0 }; // payload (byte)
};

struct C2S_LoginRequest {
        MsgType type = MsgType::C2S_LOGIN_REQUEST;

        char playerName[MAX_NAME_LEN];
        uint16_t    clientUdpPort{ 0 };
};

struct S2C_LoginAccept {
        MsgType         type = MsgType::S2C_LOGIN_ACCEPT;

        PlayerID        playerId;
};

struct C2S_MovementUpdate {
        MsgType         type = MsgType::C2S_MOVEMENT_UPDATE;

        uint32_t        msgSeq;
        PlayerID        playerId;

        Vec3            position;      // 위치
        Vec3            rotation;      // 캐릭터가 보는 방향 (회전)
        Vec3            velocity;      // 이동할 때의 속도
        uint8_t         inputKeys;
        bool            isOnGround;

        float           clientTime{ 0.f };
};

struct C2S_FireAction {
        MsgType         type = MsgType::C2S_FIRE_ACTION;

        uint32_t        msgSeq;
        PlayerID        playerId;
        Vec3            fireOrigin;
        Vec3            fireDirection;
        float           clientTime{ 0.f };

        PlayerID    hitPlayerID;
};

struct S2C_SnapshotState {
        MsgType         type = MsgType::S2C_SNAPSHOT_STATE;

        PlayerSnapshot snapshots[MAX_PLAYER_COUNT];
};

struct S2C_FireEvent {
        MsgType         type = MsgType::S2C_FIRE_EVENT;

        PlayerID        shooterID;
        Vec3            origin;
        Vec3            direction;
        PlayerID    hitPlayerID;
};
