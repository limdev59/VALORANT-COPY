#include "pch.h"
#include "CScene.h"
#include "CObject.h"

CScene::CScene()
{
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        arrObj[i] = std::vector<CObject*>();  // 각 요소를 벡터로 명시적으로 초기화
    }
}


CScene::~CScene()
{
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        for (size_t j = 0; j < arrObj[i].size(); ++j) {
            delete arrObj[i][j];
        }
        arrObj[i].clear();
    }
}

void CScene::Update()
{
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        for (size_t j = 0; j < arrObj[i].size(); ++j) {
                arrObj[i][j]->Update();
        }
    }
}

void CScene::Render()
{
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        for (size_t j = 0; j < arrObj[i].size(); ++j) {
                arrObj[i][j]->Render();
        }
    }
}


void CScene::Enter()
{
}

void CScene::Exit()
{
   
}
