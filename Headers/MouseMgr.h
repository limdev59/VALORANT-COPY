#pragma once
class MouseMgr {
public:
    static MouseMgr* Instance() {
        static MouseMgr instance;
        return &instance;
    }

    // 마우스 상태 반환 함수
    vec2 getCursorPos() const;                // 현재 마우스 위치 반환
    void setCursorPos(float x, float y);

    bool getLeftClickState() const;           // 왼쪽 버튼 클릭 상태 반환
    bool getRightClickState() const;          // 오른쪽 버튼 클릭 상태 반환

    // 초기화 및 업데이트
    void Init(int w, int h);                  // 창 크기 초기화 및 이벤트 등록
    void Update();                            // 주기적 업데이트 (현재는 비어 있음)

    // 창 크기 설정
    void setWindowSize(int w, int h);

private:
    MouseMgr();                               // 생성자
    ~MouseMgr();                              // 소멸자

    // GLUT 이벤트 핸들러
    void handleMouseClick(int button, int state, int x, int y);
    void handleMouseMove(int x, int y);

    // 멤버 변수
    int width, height;                        // 창 크기
    vec2 cursorPos;                           // 현재 마우스 위치
    bool isLeftClick;                         // 왼쪽 버튼 클릭 상태
    bool isRightClick;                        // 오른쪽 버튼 클릭 상태
};
