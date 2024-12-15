#pragma once
class MouseMgr {
public:
    static MouseMgr* Instance() {
        static MouseMgr instance;
        return &instance;
    }

    // ���콺 ���� ��ȯ �Լ�
    vec2 getCursorPos() const;                // ���� ���콺 ��ġ ��ȯ
    void setCursorPos(float x, float y);

    bool getLeftClickState() const;           // ���� ��ư Ŭ�� ���� ��ȯ
    bool getRightClickState() const;          // ������ ��ư Ŭ�� ���� ��ȯ

    // �ʱ�ȭ �� ������Ʈ
    void Init(int w, int h);                  // â ũ�� �ʱ�ȭ �� �̺�Ʈ ���
    void Update();                            // �ֱ��� ������Ʈ (����� ��� ����)

    // â ũ�� ����
    void setWindowSize(int w, int h);

private:
    MouseMgr();                               // ������
    ~MouseMgr();                              // �Ҹ���

    // GLUT �̺�Ʈ �ڵ鷯
    void handleMouseClick(int button, int state, int x, int y);
    void handleMouseMove(int x, int y);

    // ��� ����
    int width, height;                        // â ũ��
    vec2 cursorPos;                           // ���� ���콺 ��ġ
    bool isLeftClick;                         // ���� ��ư Ŭ�� ����
    bool isRightClick;                        // ������ ��ư Ŭ�� ����
};
