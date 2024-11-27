#pragma once

enum class MOUSE_TYPE {
    MOVE_HOVER = 0,
    MOVE_HOLD = 1,
    LEFT_CLICK_DOWN = 2,
    LEFT_CLICK_UP = 3,
    RIGHT_CLICK_DOWN = 4,
    RIGHT_CLICK_UP = 5,
    THUMB_BUTTON_1_DOWN = 6,
    THUMB_BUTTON_1_UP = 7,
    THUMB_BUTTON_2_DOWN = 8,
    THUMB_BUTTON_2_UP = 9,
    LAST = 10,
};

struct MouseInfo {
    vec3                cursorPos;
    bool                clicked = false;
};

class MouseMgr {
    SINGLE(MouseMgr)

private:
    int                 width;
    int                 height;
    vec3                cursorPos;
    vector<MouseInfo>   arrPos[static_cast<int>(MOUSE_TYPE::LAST)];

public:
    void Init(int w, int h);
    void Update();
    void setWindowSize(int w, int h);

    void handleMouseEvent(MOUSE_TYPE event, int x, int y);
    vec3 getMouseClick(MOUSE_TYPE event);
    vec3 getCursorPos(MOUSE_TYPE type) const;
};
