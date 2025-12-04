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
    // 1. 셰이더 및 텍스처 해제
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // [수정됨] 은면 제거(Cull Face) 끄기
    // 뒤집힌 면이나 트리거 박스 등도 모두 시각화하여 "투명 벽"의 정체를 확인합니다.
    glDisable(GL_CULL_FACE);

    // 2. 반투명 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 3. 카메라 행렬 적용
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getViewMatrix()));

    // 맵 데이터 가져오기
    const std::vector<glm::vec3>& triangles = GetAllTriangles();

    // ---------------------------------------------------------
    // [1] 면(Face) 그리기 - 반투명 파란색
    // ---------------------------------------------------------
    glDepthMask(GL_FALSE); // 투명 물체 깊이 기록 끄기

    glColor4f(0.0f, 0.0f, 1.0f, 0.3f); // 파란색, 투명도 30%

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f); // 면을 앞으로 당김

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < triangles.size(); i += 3) {
        if (i + 2 >= triangles.size()) break;
        glVertex3fv(glm::value_ptr(triangles[i]));
        glVertex3fv(glm::value_ptr(triangles[i + 1]));
        glVertex3fv(glm::value_ptr(triangles[i + 2]));
    }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_TRUE);

    // ---------------------------------------------------------
    // [2] 외곽선(Wireframe) 그리기 - 진한 파란색
    // ---------------------------------------------------------
    glLineWidth(1.5f);
    glColor4f(0.0f, 0.0f, 0.8f, 1.0f);

    glBegin(GL_LINES);
    for (size_t i = 0; i < triangles.size(); i += 3) {
        if (i + 2 >= triangles.size()) break;
        glm::vec3 v0 = triangles[i];
        glm::vec3 v1 = triangles[i + 1];
        glm::vec3 v2 = triangles[i + 2];

        glVertex3fv(glm::value_ptr(v0)); glVertex3fv(glm::value_ptr(v1));
        glVertex3fv(glm::value_ptr(v1)); glVertex3fv(glm::value_ptr(v2));
        glVertex3fv(glm::value_ptr(v2)); glVertex3fv(glm::value_ptr(v0));
    }
    glEnd();

    // 4. 상태 복구
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); // [중요] 렌더링이 끝나면 다시 켜줍니다.
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