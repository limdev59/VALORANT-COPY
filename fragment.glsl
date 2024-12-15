#version 330 core

in vec2 TexCoords;  // 텍스처 좌표
in vec3 FragPos;    // 픽셀의 월드 좌표
in vec3 Normal;     // 픽셀의 법선 벡터

out vec4 FragColor; // 최종 출력 색상


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
    // 텍스처 색상
    vec3 texColor = texture(diffuseTexture, TexCoords).rgb;
    if (length(texColor) == 0.0) {
        texColor = vec3(1.0); // 기본 색상(흰색)
    }

    // 광원 방향 및 뷰 방향 계산
    vec3 norm = normalize(Normal);  // 보간된 법선은 정규화 필요
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient (환경광)
    vec3 ambient = Ka * lightColor;

    // Diffuse (난반사광)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * lightColor ;

    // Specular (반사광)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // 최종 색상 계산
    vec3 result = (ambient + diffuse* Kd + specular)   *texColor;
    FragColor = vec4(result, 1.0); // 알파값 1.0
}

