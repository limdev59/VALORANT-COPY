#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;

 //���� ������ (��Ȱ��ȭ��)
 uniform vec3 lightPos;
 uniform vec3 viewPos;
 uniform vec3 lightColor;

void main() {
    vec3 diffuseColor = texture(diffuseTexture, TexCoord).rgb;

    vec3 normal = texture(normalMap, TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.); // [-1, 1] ������ ��ȯ

     vec3 lightDir = normalize(lightPos - FragPos);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 diffuse = diff * lightColor;

     vec3 viewDir = normalize(viewPos - FragPos);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
     vec3 specular = spec * lightColor;

    //�ؽ�ó ���� ����
    vec3 result = (diffuse + specular) * diffuseColor;
    FragColor = vec4(diffuseColor, 1.0);
}

