#include "pch.h"
#include "DebugDraw.h"
#include "CameraMgr.h"

namespace DebugDraw {
namespace {
GLuint g_hitboxVAO = 0;
GLuint g_hitboxVBO = 0;
}

void DrawAABB(GLuint shaderProgramID, const glm::vec3& min, const glm::vec3& max, const glm::vec3& color) {
    if (shaderProgramID == 0) return;

    if (g_hitboxVAO == 0) {
        glGenVertexArrays(1, &g_hitboxVAO);
        glGenBuffers(1, &g_hitboxVBO);
        glBindVertexArray(g_hitboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, g_hitboxVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }

    std::array<glm::vec3, 24> vertices = {
        glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, min.y, min.z),
        glm::vec3(min.x, min.y, min.z), glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, min.z), glm::vec3(min.x, min.y, max.z),

        glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, max.y, min.z),

        glm::vec3(min.x, max.y, min.z), glm::vec3(min.x, max.y, max.z),
        glm::vec3(min.x, max.y, min.z), glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z), glm::vec3(max.x, min.y, max.z),
        glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, max.y, max.z),
    };

    glBindVertexArray(g_hitboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_hitboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);

    glUseProgram(shaderProgramID);

    CCamera* camera = CameraMgr::Instance()->getMainCamera();
    glm::mat4 identity = glm::mat4(1.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "modelTransform");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "viewTransform");
    GLuint projLoc = glGetUniformLocation(shaderProgramID, "projectionTransform");
    GLuint colorLoc = glGetUniformLocation(shaderProgramID, "color");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    glBindVertexArray(0);
}
}

