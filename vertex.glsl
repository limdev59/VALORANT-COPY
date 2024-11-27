#version 330 core

layout (location=0) in vec3 vPos;     // ��ġ
layout(location = 1) in vec3 vColor;   // ����
layout(location = 2) in vec2 vTexCoord; // �ؽ�ó ��ǥ �߰�

out vec3 fragColor;  // ����
out vec2 fragTexCoord; // �ؽ�ó ��ǥ

uniform mat4 modelTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * modelTransform * vec4(vPos, 1.0);
    fragColor = vColor;
    fragTexCoord = vTexCoord;  // �ؽ�ó ��ǥ ����
}
