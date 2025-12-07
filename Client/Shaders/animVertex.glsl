#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal; // 원본에 선언된 변수
out mat3 TBN;

uniform mat4 modelMat;
uniform mat4 PVM;
uniform mat3 normalMat;

void main()
{
    vec4 transformedPos = vec4(0.f);
    mat4 BoneTransform = mat4(0.f);
    float totalWeight = 0.f; // [수정] 가중치 합을 추적

    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if(boneIds[i] == -1)
			continue;
		if(boneIds[i] >= MAX_BONES)
		{
            // (C++ 버그를 고쳤으므로 이 코드는 실행되지 않아야 하지만,
            // 만일을 대비한 방어 코드)
			transformedPos = vec4(pos, 1.f);
            totalWeight = 1.0;
			break;
		}
		vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.f);
		transformedPos += localPosition * weights[i];
		
		BoneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
        totalWeight += weights[i]; // [수정] 가중치 누적
	}

	mat3 normalMatrix;

    // [수정] "따로 노는" 버그의 근본 원인 해결
    // 가중치 합(totalWeight)이 0에 가까우면...
	if (totalWeight < 0.001f) 
	{
        // 1. (0,0,0) 쏠림 방지
		transformedPos = vec4(pos, 1.f);
        // 2. 'inverse(mat3(0.f))' (NaN) 방지
		normalMatrix = mat3(1.f);
	}
	else
	{
        // 정상적인 버텍스
		normalMatrix = transpose(inverse(mat3(BoneTransform)));
	}
    
    // --- 이하 원본 코드 (이제 NaN 없이 안전함) ---
    gl_Position = PVM  * transformedPos;

    FragPos = (transformedPos).xyz; 
    TexCoord = tex;
    vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * normal);
	TBN = mat3(T, B, N);
}