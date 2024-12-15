#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

const int MAX_BONES = 130;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES]; // 본 변환 행렬 (최대 100개 본)

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;

uniform mat4 modelMat;
uniform mat4 PVM;
uniform mat3 normalMat;

void main()
{
    vec4 transformedPos = vec4(0.f);
	mat4 BoneTransform = mat4(0.f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if(boneIds[i] == -1)
			continue;
		if(boneIds[i] >= MAX_BONES)
		{
			transformedPos = vec4(pos, 1.f);
			break;
		}
		vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.f);
		transformedPos += localPosition * weights[i];
		
		BoneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
	}
	mat3 normalMatrix = transpose(inverse(mat3(BoneTransform)));
    

    gl_Position = PVM  * transformedPos;

    FragPos = (transformedPos).xyz; 
    TexCoord = tex;

	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * normal);

	TBN = mat3(T, B, N);
}
