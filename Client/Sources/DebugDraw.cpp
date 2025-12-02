#include "pch.h"
#include "DebugDraw.h"
#include <vector>

namespace {
    GLuint g_hitboxVAO = 0;
    GLuint g_hitboxVBO = 0;
    GLuint g_hitboxEBO = 0;

    void EnsureHitboxBuffers() {
        if (g_hitboxVAO != 0) return;

        // Position (3) + Normal (3) + TexCoord (2)
        float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
             0.5f, -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
             0.5f,  0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
            -0.5f,  0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
            -0.5f, -0.5f,  0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
             0.5f, -0.5f,  0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
             0.5f,  0.5f,  0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
            -0.5f,  0.5f,  0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
        };

        unsigned int indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            0, 4, 1, 5, 2, 6, 3, 7
        };

        glGenVertexArrays(1, &g_hitboxVAO);
        glGenBuffers(1, &g_hitboxVBO);
        glGenBuffers(1, &g_hitboxEBO);

        glBindVertexArray(g_hitboxVAO);

        glBindBuffer(GL_ARRAY_BUFFER, g_hitboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_hitboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void DrawAABB(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color) {
    EnsureHitboxBuffers();

    GLuint shader = CCore::Instance()->shaderProgramID;
    glUseProgram(shader);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
    model = glm::scale(model, size);

    auto camera = CameraMgr::Instance()->getMainCamera();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader, "modelTransform"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewTransform"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionTransform"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec3 rgb(color);
    glUniform3fv(glGetUniformLocation(shader, "Ka"), 1, glm::value_ptr(rgb));
    glUniform3fv(glGetUniformLocation(shader, "Kd"), 1, glm::value_ptr(rgb));
    glUniform3fv(glGetUniformLocation(shader, "Ks"), 1, glm::value_ptr(glm::vec3(0.1f)));
    glUniform1f(glGetUniformLocation(shader, "Ns"), 16.0f);
    glUniform1i(glGetUniformLocation(shader, "diffuseTexture"), 0);

    glBindVertexArray(g_hitboxVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}
