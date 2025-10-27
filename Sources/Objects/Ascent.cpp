#include "pch.h"
#include "Ascent.h"
#include "IModel.h"      // IModel 구현 포함
#include "CCore.h"       // 셰이더 ID를 가져오기 위해
#include "define.h"      // MODEL_TYPE::ASCENT 열거형을 사용하기 위해

// 생성자: IModel<Model>을 생성하고 ASCENT 맵을 로드합니다.
Ascent::Ascent()
    : CObject() {
    // IModel<Model> 생성자가 SFINAE에 의해 (MODEL_TYPE, GLenum) 버전을 호출합니다.
    // Stage_1_Scene.cpp에서 ASCENT를 사용했으므로 이를 로드합니다.
    m_pModel = new IModel<Model>(MODEL_TYPE::ASCENT, GL_TRIANGLES);
}

// 소멸자: IModel 래퍼를 안전하게 삭제합니다.
Ascent::~Ascent() {
    if (m_pModel) delete m_pModel; // 래퍼가 내부의 Model*을 자동 delete 합니다.
}

// Update: CObject의 위치/회전/크기 변경을 Model 클래스 내부로 전달합니다.
void Ascent::Update() {
    if (m_pModel) {
        // Model.h에 정의된 Update 함수를 호출합니다.
        m_pModel->GetModel()->Update(position, rotation, scale);
    }

    // 히트박스 업데이트 (이전 CObject.cpp 로직)
    hitboxCenter = position;
    hitboxSize = scale;
}

// Render: 정적 모델 셰이더(shaderProgramID)를 활성화하고 모델을 렌더링합니다.
void Ascent::Render() {
    if (m_pModel) {
        // 1. 정적 모델 셰이더(shaderProgramID)를 활성화합니다.
        // (Sova/Player가 shaderProgramID2로 변경했을 수 있으므로)
        glUseProgram(CCore::Instance()->shaderProgramID);

        // 2. Model::Render() 호출
        // Model 클래스의 Render는 PVM 중 View, Proj가
        // CCore::Render()에서 이미 설정되었다고 가정하고
        // Model 행렬만 내부적으로 계산하여 렌더링합니다.
        m_pModel->GetModel()->Render(CCore::Instance()->shaderProgramID);
    }
}