#version 330 core
layout (location=0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

out vec3 fragColor;

uniform mat4 modelTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * modelTransform * vec4(vPos, 1.0);
    fragColor = vColor;
}
