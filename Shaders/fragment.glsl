#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor; 


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
        texColor = vec3(0.5);
    }

    // 광원 방향 및 뷰 방향 계산
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 반사 벡터
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient (환경광) 계산
    vec3 ambient = Ka * lightColor;

    // Diffuse (난반사광) 계산
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * lightColor ;

    float shininess = (Ns < 2.0f) ? 16.0f : Ns;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = Ks  * lightColor * spec;

    vec3 result = (ambient*1.05 + diffuse*1.4 * Kd+ specular*0.7) * texColor*1.3 ;
    FragColor = vec4(result, 1.0);
}