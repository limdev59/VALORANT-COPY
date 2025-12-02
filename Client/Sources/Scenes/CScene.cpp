#include "pch.h"
#include "CScene.h"
#include "CObject.h"
#include "CCore.h"
#include "Ascent.h"
#include <utility>

namespace {
std::pair<glm::vec3, glm::vec3> BuildAABB(const CObject& obj) {
    glm::vec3 pos = obj.getPosition();
    glm::vec3 center = obj.getHitboxCenter();
    glm::vec3 size = obj.getHitboxSize();

    glm::vec3 min = (pos + center) - (size * 0.5f);
    glm::vec3 max = (pos + center) + (size * 0.5f);
    return { min, max };
}

void DrawAABB(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color) {
    const glm::vec3 corners[8] = {
        {min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, max.y, min.z}, {min.x, max.y, min.z},
        {min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, max.y, max.z}, {min.x, max.y, max.z}
    };

    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINES);
    for (const auto& edge : edges) {
        glVertex3fv(glm::value_ptr(corners[edge[0]]));
        glVertex3fv(glm::value_ptr(corners[edge[1]]));
    }
    glEnd();
}
}

CScene::CScene()
    :loaded(false)
{
    for (GLuint i = 0; i < (GLuint)GROUP_TYPE::END; ++i) {
        arrObj[i] = std::vector<CObject*>();  //  Ҹ ͷ  ʱȭ
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

    if (CCore::Instance()->IsHitboxVisible()) {
        GLint prevProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
        glUseProgram(0);
        glLineWidth(2.0f);

        const glm::vec3 mapColor(0.0f, 1.0f, 0.0f);
        const glm::vec3 playerColor(1.0f, 0.0f, 0.0f);

        for (CObject* obj : arrObj[(GLuint)GROUP_TYPE::DEFAULT]) {
            if (auto* ascent = dynamic_cast<Ascent*>(obj)) {
                for (const auto& bounds : ascent->GetWorldColliders()) {
                    DrawAABB(bounds.min, bounds.max, mapColor);
                }
            }
            else {
                auto bounds = BuildAABB(*obj);
                DrawAABB(bounds.first, bounds.second, mapColor);
            }
        }

        for (CObject* obj : arrObj[(GLuint)GROUP_TYPE::PLAYER]) {
            auto bounds = BuildAABB(*obj);
            DrawAABB(bounds.first, bounds.second, playerColor);
        }

        glLineWidth(1.0f);
        glUseProgram(prevProgram);
    }
}


void CScene::Enter()
{
}

void CScene::Exit()
{

}
