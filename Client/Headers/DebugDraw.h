#pragma once
#include "pch.h"

namespace DebugDraw {
void DrawAABB(GLuint shaderProgramID, const glm::vec3& min, const glm::vec3& max, const glm::vec3& color);
}

