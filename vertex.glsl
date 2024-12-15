#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main() {
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(aPos, 1.0);
    FragPos = vec3(modelTransform * vec4(aPos, 1.0)); // ���� ��ǥ�� ��ȯ
    Normal = mat3(transpose(inverse(modelTransform))) * aNormal; // ���� ��ȯ
    TexCoords = aTexCoord; // �ؽ�ó ��ǥ ����
}
