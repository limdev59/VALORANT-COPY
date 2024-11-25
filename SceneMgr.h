#pragma once
class CScene;

class SceneMgr {
    SINGLE(SceneMgr);
private:
    CScene*                 arrScene[(unsigned int)SCENE_TYPE::END];
    CScene*                 currentScene;

public:
    void Init();
    void Update();
    void Render();
    void changeScene(SCENE_TYPE newScene);
};
