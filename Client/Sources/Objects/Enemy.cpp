#include "pch.h"
#include "Enemy.h"
#include "IModel.h"
#include "CCore.h"
#include "define.h"

#include "AnimModel.h"
#include "CameraMgr.h"
#include "TimeMgr.h"
#include "CCamera.h"

float IntersectRaySphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius);
float IntersectRayCylinder(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& cylBase, float height, float radius);

Enemy::Enemy()
    : CObject() {
    
    // 1. Model / Animation ε (Player ϰ )
    string modelName = "jettSimple";
    string pathName = "jettSimple";
    m_pModel = new IModel<AnimModel>(modelName, pathName);

    AnimModel* currModel = m_pModel->GetModel(); // 𵨿   

    // ִϸ̼  ε
    m_pIdleAnim = new Animation("Animations/jettSimple/jettSimple_aim.gltf", currModel);
    m_pWalkFrontAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkN.gltf", currModel);
    m_pWalkFrontLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkNW.gltf", currModel);
    m_pWalkFrontRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkNE.gltf", currModel);
    m_pWalkRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkE.gltf", currModel);
    m_pWalkLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkW.gltf", currModel);
    m_pWalkBackLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkSW.gltf", currModel);
    m_pWalkBackRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkSE.gltf", currModel);
    m_pWalkBackAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkS.gltf", currModel);

    // Animator ʱȭ (⺻ : Idle)
    m_pAnimator = new Animator(m_pIdleAnim);

    // ⺻ Transform  (Player )
    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f));
}

Enemy::~Enemy() {
    if (m_pModel) delete m_pModel;
    if (m_pAnimator) delete m_pAnimator;
    
    // ִϸ̼ ҽ 
    if (m_pIdleAnim) delete m_pIdleAnim;
    if (m_pWalkFrontAnim) delete m_pWalkFrontAnim;
    if (m_pWalkFrontLeftAnim) delete m_pWalkFrontLeftAnim;
    if (m_pWalkFrontRightAnim) delete m_pWalkFrontRightAnim;
    if (m_pWalkLeftAnim) delete m_pWalkLeftAnim;
    if (m_pWalkRightAnim) delete m_pWalkRightAnim;
    if (m_pWalkBackAnim) delete m_pWalkBackAnim;
    if (m_pWalkBackLeftAnim) delete m_pWalkBackLeftAnim;
    if (m_pWalkBackRightAnim) delete m_pWalkBackRightAnim;
}

void Enemy::Update() {
    if (m_isDead) return;

    // 1.  ġ/ȸ/ũ Ʈ (Ʈũ  position, rotation  ݿ)
    if (m_pModel) {
        m_pModel->GetModel()->UpdateTransform(position, rotation, scale);
    }
    
    Animation* targetAnim = m_pIdleAnim;
    
    bool bW = (m_inputKeys & KEY_W);
    bool bA = (m_inputKeys & KEY_A);
    bool bS = (m_inputKeys & KEY_S);
    bool bD = (m_inputKeys & KEY_D);
    
    if (m_isOnGround) {
        if (bW && bA) {
            targetAnim = m_pWalkFrontLeftAnim;  // +
        }
        else if (bW && bD) {
            targetAnim = m_pWalkFrontRightAnim; // +
        }
        else if (bS && bA) {
            targetAnim = m_pWalkBackLeftAnim;   // +
        }
        else if (bS && bD) {
            targetAnim = m_pWalkBackRightAnim;  // +
        }
        else if (bW) {
            targetAnim = m_pWalkFrontAnim;      // 
        }
        else if (bS) {
            targetAnim = m_pWalkBackAnim;       // 
        }
        else if (bA) {
            targetAnim = m_pWalkLeftAnim;       // 
        }
        else if (bD) {
            targetAnim = m_pWalkRightAnim;      // 
        }
    }
    
    //  ִϸ̼ǰ ٸ ü
    if (m_pAnimator && m_pAnimator->GetCurrAnimation() != targetAnim) {
        m_pAnimator->PlayAnimation(targetAnim);
    }

    // 2. ִϸ̼ Ʈ
    if (m_pAnimator) {
        m_pAnimator->UpdateAnimation(DT);
    }

    // 3. Ʈڽ Ʈ
    hitboxCenter = position;
    hitboxSize = scale;
}

void Enemy::Render() {
    if (m_isDead) return;

    if (m_pModel) {
        // ִϸ̼ ̴  (Player::Render )
        glUseProgram(CCore::Instance()->shaderProgramID2);
        
        AnimModel* currModel = m_pModel->GetModel();

        // Model  
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::mat4 T = glm::translate(glm::mat4(1.f), position);
        // Player  ȸ  (ʹϾ)
        glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
        glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
        modelMat = T * R * S;

        modelMat = glm::translate(modelMat, vec3(0.0f, 0.1f, 0.0f));

        // Uniform 
        mat4 projMat = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
        mat4 view = CameraMgr::Instance()->getMainCamera()->getViewMatrix();
        glm::mat4 PVM = projMat * view * modelMat;
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));

        GLuint loc_modelMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "modelMat");
        GLuint loc_PVM = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "PVM");
        GLuint loc_normalMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMat");

        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        // Bone()  Uniform  (ִϸ̼ ٽ)
        if (m_pAnimator) {
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices();
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++) {
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));
            }
        }

        // ؽó ε
        GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
        GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);

        // ׸
        currModel->Render();
    }
    if (m_bShowHitbox) {
        RenderHitbox();
    }
}

void Enemy::RenderHitbox() {
    glUseProgram(0);            // ̴ 
    glDisable(GL_LIGHTING);     //  
    glDisable(GL_TEXTURE_2D);   // ؽó 
    glDisable(GL_CULL_FACE);    //  

    // ī޶  
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getProjectionMatrix()));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getViewMatrix()));

    glPushMatrix();
    //  ġ ̵
    glTranslatef(position.x, position.y, position.z);

    // [ ]  Ʈڽ  (Red)
    glColor3f(1.0f, 0.0f, 0.0f);

    // 1. ϴ 
    glPushMatrix();
    glTranslatef(0.0f, m_radius, 0.0f);
    glutWireSphere(m_radius, 16, 16);
    glPopMatrix();

    // 2.  
    glPushMatrix();
    glTranslatef(0.0f, m_height - m_radius, 0.0f);
    glutWireSphere(m_radius, 16, 16);
    glPopMatrix();

    // 3.  (4  )
    glBegin(GL_LINES);
    {
        glVertex3f(m_radius, m_radius, 0);   glVertex3f(m_radius, m_height - m_radius, 0);
        glVertex3f(-m_radius, m_radius, 0);  glVertex3f(-m_radius, m_height - m_radius, 0);
        glVertex3f(0, m_radius, m_radius);   glVertex3f(0, m_height - m_radius, m_radius);
        glVertex3f(0, m_radius, -m_radius);  glVertex3f(0, m_height - m_radius, -m_radius);
    }
    glEnd();

    glPopMatrix();

    //  
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); //  ʱȭ
}

void Enemy::TakeDamage(int damage) {
    m_health -= damage;
    if (m_health <= 0) {
        OnDeath();
    }
}

void Enemy::OnDeath() {
    if (m_isDead) return;

    m_isDead = true;
    m_health = 0;
    std::cout << "Enemy died!" << std::endl;
}

float Enemy::CheckRayHit(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
    // 1. Ʈڽ  (World Space)
    // RenderHitbox   
    glm::vec3 basePos = this->position + glm::vec3(0.0f, m_radius, 0.0f);           // ϴ  ߽
    glm::vec3 topPos  = this->position + glm::vec3(0.0f, m_height - m_radius, 0.0f); //   ߽
    float cylinderHeight = (m_height - m_radius) - m_radius; //   

    float closestDist = 99999.0f;
    bool hit = false;

    // 2. ϴ  ˻
    float tSphere1 = IntersectRaySphere(rayOrigin, rayDir, basePos, m_radius);
    if (tSphere1 > 0.0f && tSphere1 < closestDist) {
        closestDist = tSphere1;
        hit = true;
    }

    // 3.   ˻
    float tSphere2 = IntersectRaySphere(rayOrigin, rayDir, topPos, m_radius);
    if (tSphere2 > 0.0f && tSphere2 < closestDist) {
        closestDist = tSphere2;
        hit = true;
    }

    // 4.   ˻
    float tCyl = IntersectRayCylinder(rayOrigin, rayDir, basePos, cylinderHeight, m_radius);
    if (tCyl > 0.0f && tCyl < closestDist) {
        closestDist = tCyl;
        hit = true;
    }

    return hit ? closestDist : -1.0f;
}

void Enemy::SyncNetworkState(const PlayerSnapshot& snap) {
    bool wasDead = m_isDead;

    this->position.x = snap.position.x;
    this->position.y = snap.position.y;
    this->position.z = snap.position.z;

    this->rotation.x = snap.rotation.x;
    this->rotation.y = snap.rotation.y;
    this->rotation.z = snap.rotation.z;

    this->m_velocity.x = snap.velocity.x;
    this->m_velocity.y = snap.velocity.y;
    this->m_velocity.z = snap.velocity.z;
    m_inputKeys = snap.inputKeys;
    m_isOnGround = snap.isOnGround;

    m_health = static_cast<int>(snap.health);
    m_isDead = !snap.isAlive || m_health <= 0;

    if (m_isDead && !wasDead) {
        OnDeath();
    }
}

float IntersectRaySphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius) {
    glm::vec3 oc = rayOrigin - sphereCenter;
    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return -1.0f;
    else return (-b - sqrt(discriminant)) / (2.0f * a);
}

// -(Y ) 浹  Լ
float IntersectRayCylinder(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& cylBase, float height, float radius) {
    glm::vec3 d = rayDir;
    glm::vec3 o = rayOrigin - cylBase; //  ظ   ǥ

    // x, z 鿡    Ǻ (a*t^2 + b*t + c = 0)
    float a = d.x * d.x + d.z * d.z;
    float b = 2.0f * (o.x * d.x + o.z * d.z);
    float c = o.x * o.x + o.z * o.z - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return -1.0f;

    //    t 
    float t = (-b - sqrt(discriminant)) / (2.0f * a);
    
    // (y)  üũ
    float yHit = o.y + t * d.y;
    if (yHit >= 0.0f && yHit <= height) {
        return t;
    }
    return -1.0f;
}