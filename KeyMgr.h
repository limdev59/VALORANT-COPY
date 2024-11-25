#pragma once

enum class KEY {
    LEFT, RIGHT, UP, DOWN,
    Q, W, E, R, T, Y, U, I, O, P,
    A, S, D, F, G, H, J, K, L,
    Z, X, C, V, B, N, M,
    SPACE, ENTER, ESC,
    NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
    NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, // �߰��� ���Ű
    HOME, END, PAGE_UP, PAGE_DOWN, // �߰��� Ư�� Ű
    PLUS, MINUS, ASTERISK, SLASH, // ������ Ű
    CTRL, SHIFT, ALT, // ���� Ű (��Ʈ��, ����Ʈ, ��Ʈ)
    LAST
};

enum class KEY_TYPE {
    NONE,
    TAP,
    HOLD,
    AWAY
};

struct keyInfo {
    KEY_TYPE state;
    bool prev;
};

class KeyMgr {
    SINGLE(KeyMgr)
private:
    std::vector<keyInfo>    vecKey;
public:
    void                    Init();
    void                    Update();
    KEY_TYPE                getKeyState(KEY key) { return vecKey[(int)key].state; }

    static void             KeyboardDown(unsigned char key, int x, int y);
    static void             KeyboardUp(unsigned char key, int x, int y);
    static void             SpecialDown(int key, int x, int y);
    static void             SpecialUp(int key, int x, int y);
};
