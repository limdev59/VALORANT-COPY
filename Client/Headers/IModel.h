#pragma once
#include "Model.h"      // 정적 모델 클래스
#include "AnimModel.h"  // 애니메이션 모델 클래스
#include <type_traits>  // std::enable_if_t, std::is_same_v 사용

/**
 * @class IModel
 * @brief Model(정적) 또는 AnimModel(애니메이션) 객체를 템플릿을 통해 생성하고 소유권을 관리하는 래퍼 클래스입니다.
 *
 * @tparam T Model 또는 AnimModel 클래스 타입
 *
 * @details
 * SFINAE (std::enable_if_t)를 활용하여 T 타입에 따라
 * 올바른 생성자(모델 로딩 방식)만 활성화합니다.
 */
template <typename T>
class IModel {
private:
    T* m_pModel; // 템플릿 타입(Model 또는 AnimModel)의 포인터

public:
    /**
     * @brief [SFINAE] T가 Model일 경우에만 활성화되는 생성자입니다.
     * @details 정적 Model(OBJ 로더)을 생성합니다.
     */
    template <typename T_ = T,
        std::enable_if_t<std::is_same_v<T_, Model>, int> = 0>
    IModel(MODEL_TYPE type, GLenum mode) {
        // Model(MODEL_TYPE, GLenum) 생성자 호출
        m_pModel = new Model(type, mode);
    }

    /**
     * @brief [SFINAE] T가 AnimModel일 경우에만 활성화되는 생성자입니다.
     * @details 애니메이션 AnimModel(gltf 로더)을 생성하고 로드합니다.
     */
    template <typename T_ = T,
        std::enable_if_t<std::is_same_v<T_, AnimModel>, int> = 0>
    IModel(const std::string& fileName) {
        // AnimModel의 기본 생성자 호출 후 LoadModel 실행
        m_pModel = new AnimModel();
        m_pModel->LoadModel(fileName);
    }

    /**
     * @brief 소멸자. 관리하던 모델 객체를 안전하게 해제합니다.
     */
    ~IModel() {
        if (m_pModel) {
            delete m_pModel;
            m_pModel = nullptr;
        }
    }

    /**
     * @brief 관리 중인 모델 객체의 포인터를 반환합니다.
     * @return T* (Model* 또는 AnimModel*)
     */
    T* GetModel() const {
        return m_pModel;
    }

    /**
     * @brief GetModel()과 동일합니다. 포인터처럼 사용할 수 있도록 오버로딩합니다.
     */
    T* operator->() const {
        return m_pModel;
    }

    // --- 복사 및 이동 금지 ---
    IModel(const IModel&) = delete;
    IModel& operator=(const IModel&) = delete;
    IModel(IModel&&) = delete;
    IModel& operator=(IModel&&) = delete;
};