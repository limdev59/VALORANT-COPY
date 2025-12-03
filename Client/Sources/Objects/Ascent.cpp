#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel 구현 포함
#include "CCore.h"       // 셰이더 ID를 가져오기 위해
#include "define.h"      // MODEL_TYPE::ASCENT 열거형을 사용하기 위해
#include "CameraMgr.h"
#include "CCamera.h"

// 생성자: IModel<Model>을 생성하고 ASCENT 맵을 로드합니다.
Ascent::Ascent()
    : m_pModelDecoration(nullptr)
    , m_pModelFloor(nullptr)
    , m_pModelWall(nullptr)
    , m_pModelProps(nullptr)
{
    m_pModelDecoration  = new IModel<Model>(MODEL_TYPE::ASCENT_DECORATION, GL_TRIANGLES);
    m_pModelFloor       = new IModel<Model>(MODEL_TYPE::ASCENT_FLOOR, GL_TRIANGLES);
    m_pModelWall        = new IModel<Model>(MODEL_TYPE::ASCENT_WALL, GL_TRIANGLES);
    m_pModelProps       = new IModel<Model>(MODEL_TYPE::ASCENT_PROPS, GL_TRIANGLES);
}

// 소멸자: IModel 래퍼를 안전하게 삭제합니다.
Ascent::~Ascent() {
    if (m_pModelDecoration) delete m_pModelDecoration;
    if (m_pModelFloor) delete m_pModelFloor;
    if (m_pModelWall) delete m_pModelWall;
    if (m_pModelProps) delete m_pModelProps;
}

void Ascent::Update() {
    // CObject가 가지고 있는 위치, 회전, 크기 정보를 가져옴
    glm::vec3 pos = getPosition();
    glm::vec3 rot = getRotation();
    glm::vec3 scl = getScale();

    // 각 내부 모델들에게 변환 정보 업데이트 요청
    if (m_pModelDecoration) m_pModelDecoration->GetModel()->Update(pos, rot, scl);
    if (m_pModelFloor)      m_pModelFloor->GetModel()->Update(pos, rot, scl);
    if (m_pModelWall)       m_pModelWall->GetModel()->Update(pos, rot, scl);
    if (m_pModelProps)      m_pModelProps->GetModel()->Update(pos, rot, scl);
}

// Render: 정적 모델 셰이더(shaderProgramID)를 활성화하고 모델을 렌더링합니다.
void Ascent::Render() {
    // 일반 렌더링
    if (m_pModelDecoration) {
        glUseProgram(CCore::Instance()->shaderProgramID);

        m_pModelDecoration->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelFloor->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelWall->GetModel()->Render(CCore::Instance()->shaderProgramID);
        m_pModelProps->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
    
    // 히트박스 시각화 (V키 토글 시)
    if (m_bShowHitbox) {
        RenderHitbox();
    }
}

void Ascent::RenderHitbox() {
    // 1. 셰이더 해제 (단색 라인 드로잉을 위해 고정 파이프라인 사용)
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // 2. 파란색 선 설정
    glLineWidth(2.0f); // 선을 좀 더 굵게
    glColor3f(0.0f, 0.0f, 1.0f); // 파란색 (R, G, B)

    // 3. 카메라 행렬 적용
    // (Model::Render를 통하지 않으므로 직접 행렬을 로드해야 함)
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getProjectionMatrix()));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getViewMatrix()));

    // 4. 충돌체(Floor, Wall, Props)의 모든 삼각형 정점 가져오기
    std::vector<glm::vec3> triangles = GetAllTriangles();

    // 5. 와이어프레임 그리기 (GL_LINES)
    glBegin(GL_LINES);
    for (size_t i = 0; i < triangles.size(); i += 3) {
        if (i + 2 >= triangles.size()) break;

        glm::vec3 v0 = triangles[i];
        glm::vec3 v1 = triangles[i + 1];
        glm::vec3 v2 = triangles[i + 2];

        // 삼각형의 세 변(Edge)을 그립니다.
        glVertex3fv(glm::value_ptr(v0)); glVertex3fv(glm::value_ptr(v1));
        glVertex3fv(glm::value_ptr(v1)); glVertex3fv(glm::value_ptr(v2));
        glVertex3fv(glm::value_ptr(v2)); glVertex3fv(glm::value_ptr(v0));
    }
    glEnd();

    // 6. 상태 복구
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

const std::vector<glm::vec3>& Ascent::GetAllTriangles() {
    // 이미 생성되어 있고 갱신이 필요 없다면 캐시 반환
    if (!m_bIsCacheDirty && !m_cachedTriangles.empty()) {
        return m_cachedTriangles;
    }

    // 캐시 재생성
    m_cachedTriangles.clear();
    std::vector<glm::vec3> part;

    if (m_pModelFloor) {
        part = m_pModelFloor->GetModel()->GetVerticesWorld();
        m_cachedTriangles.insert(m_cachedTriangles.end(), part.begin(), part.end());
    }
    if (m_pModelWall) {
        part = m_pModelWall->GetModel()->GetVerticesWorld();
        m_cachedTriangles.insert(m_cachedTriangles.end(), part.begin(), part.end());
    }
    if (m_pModelProps) {
        part = m_pModelProps->GetModel()->GetVerticesWorld();
        m_cachedTriangles.insert(m_cachedTriangles.end(), part.begin(), part.end());
    }

    m_bIsCacheDirty = false;
    return m_cachedTriangles;
}