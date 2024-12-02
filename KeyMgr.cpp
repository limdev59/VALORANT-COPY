#include "pch.h"
#include "KeyMgr.h"

int arrVK[(int)KEY::LAST] = {
    GLUT_KEY_LEFT, GLUT_KEY_RIGHT, GLUT_KEY_UP, GLUT_KEY_DOWN, //����Ű
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    ' ',  // SPACE
    13,   // ENTER
    27,   // ESC
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  // ���� Ű
    GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, // F1~F12
    GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN,  // Ư�� Ű
    '=', '-', '*', '/',  // ������ Ű
    100, 101, 102,       // CTRL, SHIFT, ALT
};

KeyMgr::KeyMgr() {}

KeyMgr::~KeyMgr() {}

void KeyMgr::Init() {
    vecKey.resize((int)KEY::LAST, keyInfo{ KEY_TYPE::NONE, false });
    glutKeyboardFunc(KeyMgr::KeyboardDown);
    glutKeyboardUpFunc(KeyMgr::KeyboardUp);
    glutSpecialFunc(KeyMgr::SpecialDown);
    glutSpecialUpFunc(KeyMgr::SpecialUp);
}

void KeyMgr::Update() {
    for (int i = 0; i < (int)KEY::LAST; ++i) {
        if (vecKey[i].prev) {  // Ű�� ���� ����
            if (vecKey[i].state == KEY_TYPE::NONE) {
                vecKey[i].state = KEY_TYPE::TAP;  // ó�� ���� ����
            }
            else if (vecKey[i].state == KEY_TYPE::TAP) {
                vecKey[i].state = KEY_TYPE::HOLD;  // ��� ������ �ִ� ����
            }
        }
        else {  // Ű�� ������ ����
            if (vecKey[i].state == KEY_TYPE::HOLD || vecKey[i].state == KEY_TYPE::TAP) {
                vecKey[i].state = KEY_TYPE::AWAY;  // Ű�� ��� ������ ����
            }
            else if (vecKey[i].state == KEY_TYPE::AWAY) {
                vecKey[i].state = KEY_TYPE::NONE;  // ����� ���¿��� �ʱ�ȭ
            }
        }
    }
}

void KeyMgr::KeyboardDown(unsigned char key, int x, int y) {
    key = std::toupper(key);

    if (key == '+' || key == '=') key = '=';
    if (key == '-' || key == '_') key = '-';

    for (int i = 0; i < (int)KEY::LAST; ++i) {
        if (arrVK[i] == key) {
            KeyMgr::Instance()->vecKey[i].prev = true;  // ���� ���·� ����
        }
    }
}

void KeyMgr::KeyboardUp(unsigned char key, int x, int y) {
    key = std::toupper(key);

    if (key == '+' || key == '=') key = '=';
    if (key == '-' || key == '_') key = '-';

    for (int i = 0; i < (int)KEY::LAST; ++i) {
        if (arrVK[i] == key) {
            KeyMgr::Instance()->vecKey[i].prev = false;  // ������ ���·� ����
        }
    }
}

void KeyMgr::SpecialDown(int key, int x, int y) {
    // ȭ��ǥ Ű ó��
    for (int i = 0; i < (int)KEY::LAST; ++i) {
        if (arrVK[i] == key) {
            KeyMgr::Instance()->vecKey[i].prev = true;  // ���� ���·� ����
        }
    }

    // Modifier Ű ó�� (ȭ��ǥ Ű�� ���������� �����ϵ��� ����)
    int modifiers = glutGetModifiers();
    if (modifiers & GLUT_ACTIVE_CTRL) {
        KeyMgr::Instance()->vecKey[(int)KEY::CTRL].prev = true;
    }
    else {
        KeyMgr::Instance()->vecKey[(int)KEY::CTRL].prev = false;
    }

    if (modifiers & GLUT_ACTIVE_SHIFT) {
        KeyMgr::Instance()->vecKey[(int)KEY::SHIFT].prev = true;
    }
    else {
        KeyMgr::Instance()->vecKey[(int)KEY::SHIFT].prev = false;
    }

    if (modifiers & GLUT_ACTIVE_ALT) {
        KeyMgr::Instance()->vecKey[(int)KEY::ALT].prev = true;
    }
    else {
        KeyMgr::Instance()->vecKey[(int)KEY::ALT].prev = false;
    }
}



void KeyMgr::SpecialUp(int key, int x, int y) {
    for (int i = 0; i < (int)KEY::LAST; ++i) {
        if (arrVK[i] == key) {
            KeyMgr::Instance()->vecKey[i].prev = false;  // ������ ���·� ����
        }
    }

    // Modifier Keys (CTRL, SHIFT, ALT) ��Ȱ��ȭ
    KeyMgr::Instance()->vecKey[(int)KEY::CTRL].prev = false;
    KeyMgr::Instance()->vecKey[(int)KEY::SHIFT].prev = false;
    KeyMgr::Instance()->vecKey[(int)KEY::ALT].prev = false;
}
