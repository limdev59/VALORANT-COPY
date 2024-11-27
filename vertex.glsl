#version 330 core

layout (location=0) in vec3 vPos;     // 위치
layout(location = 1) in vec3 vColor;   // 색상
layout(location = 2) in vec2 vTexCoord; // 텍스처 좌표 추가

out vec3 fragColor;  // 색상
out vec2 fragTexCoord; // 텍스처 좌표

uniform mat4 modelTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * modelTransform * vec4(vPos, 1.0);
    fragColor = vColor;
    fragTexCoord = vTexCoord;  // 텍스처 좌표 전달
}
