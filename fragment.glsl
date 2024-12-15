#version 330 core

in vec2 TexCoords;  // �ؽ�ó ��ǥ
in vec3 FragPos;    // �ȼ��� ���� ��ǥ
in vec3 Normal;     // �ȼ��� ���� ����

out vec4 FragColor; // ���� ��� ����


uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform sampler2D diffuseTexture;

// Lighting
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // �ؽ�ó ����
    vec3 texColor = texture(diffuseTexture, TexCoords).rgb;
    if (length(texColor) == 0.0) {
        texColor = vec3(1.0); // �⺻ ����(���)
    }

    // ���� ���� �� �� ���� ���
    vec3 norm = normalize(Normal);  // ������ ������ ����ȭ �ʿ�
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient (ȯ�汤)
    vec3 ambient = Ka * lightColor;

    // Diffuse (���ݻ籤)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * lightColor ;

    // Specular (�ݻ籤)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // ���� ���� ���
    vec3 result = (ambient + diffuse* Kd + specular)   *texColor;
    FragColor = vec4(result, 1.0); // ���İ� 1.0
}

