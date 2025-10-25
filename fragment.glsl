#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor; 


uniform vec3 Ka; // [1단계] 주석 해제
// uniform vec3 Kd;
// uniform vec3 Ks;
// uniform float Ns;

uniform sampler2D diffuseTexture;

// Lighting
// uniform vec3 lightPos;
uniform vec3 lightColor; // [1단계] 주석 해제
// uniform vec3 viewPos;

void main() {
    // 텍스처 색상
    vec3 texColor = texture(diffuseTexture, TexCoords).rgb;
    if (length(texColor) == 0.0) {
        texColor = vec3(1.0); // 기본 색상(흰색)
    }

    // [1단계] Ambient (환경광) 계산
    vec3 ambient = Ka * lightColor;

    // [1단계] 원본 로직을 따라 앰비언트와 텍스처를 곱함
    vec3 result = ambient * texColor;
    FragColor = vec4(result, 1.0); // 알파값 1.0


    /* --- 2단계에서 여기를 수정할 것입니다 ---
    
    // 광원 방향 및 뷰 방향 계산
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Diffuse (난반사광)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * lightColor ;

    // Specular (반사광)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor;

    // 최종 색상 계산 (원본)
    // vec3 result = (ambient + diffuse* Kd + specular)   *texColor;
    // FragColor = vec4(result, 1.0); 
    */
}