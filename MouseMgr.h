#pragma once
//#include "Camera.h"
class MouseMgr {
    SINGLE(MouseMgr)
private:
    static void     MouseCallback(int button, int state, int x, int y);
public:
    //Camera          camera;
    int             width;
    int             height;
    vec3       clickDownPos;
    vec3       clickUpPos;

    bool            clicked = false;
    void            Init(int w, int h/*, Camera& cam*/);
    void            Update(/*Camera& cam*/);
    void            SetWindowSize(int w, int h);
    bool            WasClicked();
    static void     setClickDownPos(int x, int y);
    static void     setClickUpPos(int x, int y); 
    vec3            getClickDownPos() const;
    vec3            getClickUpPos() const;
    
};
