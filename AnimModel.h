#pragma once
#include "pch.h"
#include "assimp/anim.h"
#include "stb_image.h"
#include <chrono>

class AssimpGLMHelpers
{
public:

	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};
struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};
struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};
struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};
struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};
class Mesh
{
public:
	Mesh()
	{
		VAO = 0;
		VBO = 0;
		IBO = 0;
		indexCount = 0;
		name = "";
	}

	void CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::string name)
	{
		this->name = name;

		indexCount = indices.size();

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(0);

		// texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));
		glEnableVertexAttribArray(1);

		// normal
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
		glEnableVertexAttribArray(2);

		// tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Tangent)));
		glEnableVertexAttribArray(3);

		// bitangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Bitangent)));
		glEnableVertexAttribArray(4);

		// ids
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		glEnableVertexAttribArray(5);

		// weights
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		glEnableVertexAttribArray(6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	void RenderMesh()
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	void ClearMesh()
	{
		if (IBO != 0)
		{
			glDeleteBuffers(1, &IBO);
			IBO = 0;
		}

		if (VBO != 0)
		{
			glDeleteBuffers(1, &VBO);
			VBO = 0;
		}

		if (VAO != 0)
		{
			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}

		indexCount = 0;
	}

	std::string& GetName() { return name; }

	~Mesh()
	{
		ClearMesh();
	}

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
	std::string name;
};
class Material
{
public:
	Material(GLfloat map_Ks, GLfloat map_Ns) : map_Ks(map_Ks), map_Ns(map_Ns) {}
	GLfloat map_Ks; //specular
	GLfloat map_Ns; //shininess
};
class Bone
{
public:
	Bone(const std::string& name, int id, const aiNodeAnim* channel) :
		name(name), id(id), localTransform(1.f)
	{
		nPos = channel->mNumPositionKeys;
		for (int positionIndex = 0; positionIndex < nPos; positionIndex++)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			posArr.push_back(data);
		}

		nRot = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < nRot; rotationIndex++)
		{
			aiQuaternion aiRotation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(aiRotation);
			data.timeStamp = timeStamp;
			rotArr.push_back(data);
		}

		nScale = channel->mNumScalingKeys;
		for (int scaleIndex = 0; scaleIndex < nScale; scaleIndex++)
		{
			aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
			float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(aiScale);
			data.timeStamp = timeStamp;
			scales.push_back(data);
		}
	}
	void Update(float animationTime)
	{
		glm::mat4 T = InterpolatePosition(animationTime);
		glm::mat4 R = InterpolateRotation(animationTime);
		glm::mat4 S = InterpolateScale(animationTime);
		localTransform = T * R * S;
	}
	glm::mat4 GetLocalTransform() { return localTransform; }
	std::string GetBoneName() const { return name; }
	int GetBoneID() { return id; }
	int GetPositionIndex(float animationTime)
	{
		for (int i = 0; i < nPos - 1; i++)
		{
			if (animationTime < posArr[i + 1].timeStamp)
				return i;
		}
		assert(0);
	}
	int GetRotationIndex(float animationTime)
	{
		for (int i = 0; i < nRot - 1; i++)
			if (animationTime < rotArr[i + 1].timeStamp)
				return i;
		assert(0);
	}
	int GetScaleIndex(float animationTime)
	{
		for (int i = 0; i < nScale - 1; i++)
			if (animationTime < scales[i + 1].timeStamp)
				return i;
		assert(0);
	}
private:
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}
	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (nPos == 1) return glm::translate(glm::mat4(1.f), posArr[0].position);
		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(posArr[p0Index].timeStamp,
			posArr[p1Index].timeStamp, animationTime);
		glm::vec3 finalPosition = glm::mix(posArr[p0Index].position,
			posArr[p1Index].position, scaleFactor);
		return glm::translate(glm::mat4(1.f), finalPosition);
	}
	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (nRot == 1)
		{
			auto rotation = glm::normalize(rotArr[0].orientation);
			return glm::mat4_cast(rotation);
		}


		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(rotArr[p0Index].timeStamp,
			rotArr[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(rotArr[p0Index].orientation,
			rotArr[p1Index].orientation, scaleFactor);
		return glm::mat4_cast(finalRotation);
	}
	glm::mat4 InterpolateScale(float animationTime)
	{
		if (nScale == 1)
			return glm::scale(glm::mat4(1.f), scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
			scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(scales[p0Index].scale,
			scales[p1Index].scale, scaleFactor);
		return glm::scale(glm::mat4(1.f), finalScale);
	}


	std::vector<KeyPosition> posArr;
	std::vector<KeyRotation> rotArr;
	std::vector<KeyScale> scales;
	int nPos, nRot, nScale;

	glm::mat4 localTransform;
	std::string name;
	int id;
};
struct BoneInfo
{
	int id;
	glm::mat4 offset;
};
struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childcount;
	std::vector<AssimpNodeData> children;
};

class Texture
{
public:
	Texture(const char* fileLoc)
	{
		textureID = 0;
		width = 0;
		height = 0;
		bitDepth = 0;
		fileLocation = fileLoc;
	}
	bool LoadTexture(int nChannels = 0, bool preserveData = false)
	{
		unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, nChannels);
		if (!texData)
		{
			printf("Failed to find: %s\n", fileLocation);
			return false;
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int format = -1;
		switch (nChannels)
		{
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cout << "Invalid number of channels" << std::endl;
			return false;
		}

		glTexImage2D(
			GL_TEXTURE_2D, 0, format,
			width, height, 0,
			format, GL_UNSIGNED_BYTE, texData);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (preserveData)
		{
			this->texData = texData;
		}
		else
		{
			stbi_image_free(texData);
		}
		return true;
	}
	void UseTexture(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	void ClearTexture()
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
		width = 0;
		height = 0;
		bitDepth = 0;
		fileLocation = "";
	}

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetBitDepth() { return bitDepth; }
	unsigned char* GetTexData() { return texData; }

	~Texture()
	{
		ClearTexture();
	}

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
	unsigned char* texData;
};
class AnimModel {
public:
	AnimModel()
	{
		translate = glm::vec3(0.f, 0.f, 0.f);
		rotate = glm::vec3(-90.f, 0.f, 0.f);
		scale = glm::vec3(1.f, 1.f, 1.f);

		modelMat = GetModelMat();

		material = nullptr;
	}
	void LoadModel(const std::string& fileName)
	{
		int firstSlashIdx = fileName.find('/', 0);
		modelName = fileName.substr(0, firstSlashIdx);

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile("Models/" + fileName + "/" + fileName + ".gltf",
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace);

		if (!scene)
		{
			std::cout << fileName << " Failed to load model : " << importer.GetErrorString() << std::endl;
			return;
		}

		LoadNode(scene->mRootNode, scene);
		LoadMaterials(scene);
	}
	void RenderModel()
	{
		std::vector<std::pair<Mesh*, unsigned int>> solidMeshList;
		std::vector<std::pair<Mesh*, unsigned int>> transparentMeshList;

		for (size_t i = 0; i < meshList.size(); i++)
		{
			unsigned int materialIndex = meshToTex[i];
			if (meshList[i]->GetName().find("Hair") != std::string::npos ||
				meshList[i]->GetName().find("facial_serious10") != std::string::npos ||
				meshList[i]->GetName().find("facial_normal9") != std::string::npos)
				transparentMeshList.push_back({ meshList[i], materialIndex });
			else
				solidMeshList.push_back({ meshList[i], materialIndex });
		}

		// 불투명 메시 렌더링
		for (auto& item : solidMeshList)
		{
			int materialIndex = item.second;
			Mesh* mesh = item.first;

			if (materialIndex < diffuseMaps.size() && diffuseMaps[materialIndex])
				diffuseMaps[materialIndex]->UseTexture(GL_TEXTURE0);
			if (materialIndex < normalMaps.size() && normalMaps[materialIndex])
				normalMaps[materialIndex]->UseTexture(GL_TEXTURE1);

			mesh->RenderMesh();
		}

		// 투명 메시 렌더링
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (auto& item : transparentMeshList)
		{
			int materialIndex = item.second;
			Mesh* mesh = item.first;

			if (materialIndex < diffuseMaps.size() && diffuseMaps[materialIndex])
				diffuseMaps[materialIndex]->UseTexture(GL_TEXTURE0);
			if (materialIndex < normalMaps.size() && normalMaps[materialIndex])
				normalMaps[materialIndex]->UseTexture(GL_TEXTURE1);

			mesh->RenderMesh();
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	void ClearModel()
	{
		for (size_t i = 0; i < meshList.size(); i++)
		{
			if (meshList[i])
			{
				delete meshList[i];
				meshList[i] = nullptr;
			}
		}

		for (size_t i = 0; i < diffuseMaps.size(); i++)
		{
			if (diffuseMaps[i])
			{
				delete diffuseMaps[i];
				diffuseMaps[i] = nullptr;
			}
		}

		delete(material);
	}
	GLfloat* GetTranslate() { return &translate[0]; }
	GLfloat* GetRotate() { return &rotate[0]; }
	GLfloat* GetScale() { return &scale[0]; }
	void SetTranslate(glm::vec3 translate) { this->translate = translate; }
	void SetRotate(glm::vec3 rotate) { this->rotate = rotate; }
	void SetScale(glm::vec3 scale) { this->scale = scale; }
	Material* GetMaterial() { return material; }
	std::map<std::string, BoneInfo>& GetBoneInfoMap() { return boneInfoMap; }
	int& GetBoneCount() { return boneCounter; }
	glm::mat4 GetModelMat()
	{
		// model Matrix 구성
		glm::mat4 T = glm::translate(glm::mat4(1.f), glm::vec3(translate[0], translate[1], translate[2]));
		glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotate[0]), glm::radians(rotate[1]), glm::radians(rotate[2]))));
		glm::mat4 S = glm::scale(glm::mat4(1.f), glm::vec3(scale[0], scale[1], scale[2]));
		return modelMat = T * R * S;
	}
	void UpdateTransform(glm::mat4 newModelMat)
	{
		glm::vec3 translation, rotation, scale;
		SetTranslate(translation);
		SetRotate(rotation);
		SetScale(scale);
	}
private:
	void LoadMesh(aiMesh* mesh, const aiScene* scene)
	{

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
			{
				vertex.m_BoneIDs[i] = -1;
				vertex.m_Weights[i] = 0.f;
			}
			// VertexPos
			vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
			// TextCoord
			if (mesh->mTextureCoords[0]) {
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f); // Default value if no texture coordinates
			}
			// Normal
			vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
			// Tangent, Bitangent
			vertex.Tangent = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]);
			vertex.Bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);
			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

			if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
				BoneInfo boneInfo;
				boneInfo.id = boneCounter;
				auto offsetMat = mesh->mBones[boneIndex]->mOffsetMatrix;
				boneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
					mesh->mBones[boneIndex]->mOffsetMatrix
				);
				boneInfoMap[boneName] = boneInfo;
				boneID = boneCounter;
				boneCounter++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;
			for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());

				for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
				{
					if (vertices[vertexId].m_BoneIDs[i] < 0)
					{
						vertices[vertexId].m_Weights[i] = weight;
						vertices[vertexId].m_BoneIDs[i] = boneID;
						break;
					}
				}
			}

			Mesh* newMesh = new Mesh();
			newMesh->CreateMesh(vertices, indices, mesh->mName.C_Str()); // GPU의 VBO, IBO로 버텍스 정보를 쏴준다.
			meshList.push_back(newMesh);

			// meshList에 mesh를 채워줌과 동시에, meshToTex에는 그 mesh의 materialIndex를 채워준다.
			// 이렇게 meshList와 meshToTex를 나란히 채워줌으로써 mesh와 맞는 material을 손쉽게 찾을 수 있다.
			meshToTex.push_back(mesh->mMaterialIndex);
		}
		int boneCounter = 0;
		std::map<std::string, BoneInfo>  boneInfoMap;

	}
	void LoadNode(aiNode* node, const aiScene* scene)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			LoadNode(node->mChildren[i], scene);
		}
	}
	void LoadMaterials(const aiScene* scene)
	{
		diffuseMaps.resize(scene->mNumMaterials);
		normalMaps.resize(scene->mNumMaterials);
		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* material = scene->mMaterials[i];

			diffuseMaps[i] = nullptr;
			normalMaps[i] = nullptr;

			LoadDiffuseMaps(material, i);
			LoadNormalMaps(material, i);
		}

		material = new Material(0.3f, 64.f);
	}
	std::unordered_map<std::string, Texture*> textureCache;

	Texture* LoadTextureWithCaching(const std::string& texPath, int channels) {
		// 캐시에서 텍스처 검색
		auto it = textureCache.find(texPath);
		if (it != textureCache.end()) {
			std::cout << "Using cached texture: " << texPath << std::endl;
			return it->second;
		}

		// 캐시에 없으면 새로 로드
		Texture* texture = new Texture(texPath.c_str());
		std::cout << "Loading texture: " << texPath << std::endl;

		if (!texture->LoadTexture(channels)) { // 실패 시
			std::cout << "Failed to load texture: " << texPath << std::endl;
			delete texture;
			return nullptr;
		}

		// 성공적으로 로드되면 캐시에 저장
		textureCache[texPath] = texture;
		return texture;
	}

	void LoadDiffuseMaps(aiMaterial* material, const size_t& i) {
		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
			aiString texturePath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) {
				int idx = std::string(texturePath.data).rfind("/");
				std::string textureName = std::string(texturePath.data).substr(idx + 1);
				idx = std::string(textureName).rfind("\\");
				textureName = std::string(textureName).substr(idx + 1);

				std::string texPath = "Models/" + modelName + "/textures/" + textureName;

				// 캐시를 활용하여 텍스처 로드
				diffuseMaps[i] = LoadTextureWithCaching(texPath, 4);
			}
		}
	}

	void LoadNormalMaps(aiMaterial* material, const size_t& i) {
		if (material->GetTextureCount(aiTextureType_NORMALS) || material->GetTextureCount(aiTextureType_HEIGHT)) {
			aiString texturePath;
			if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == aiReturn_SUCCESS ||
				material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == aiReturn_SUCCESS) {
				int idx = std::string(texturePath.data).rfind("/");
				std::string textureName = std::string(texturePath.data).substr(idx + 1);
				idx = std::string(textureName).rfind("\\");
				textureName = std::string(textureName).substr(idx + 1);

				std::string texPath = "Models/" + modelName + "/textures/" + textureName;

				// 캐시를 활용하여 텍스처 로드
				normalMaps[i] = LoadTextureWithCaching(texPath, 3);
			}
		}
	}
	std::string name;
	glm::mat4	modelMat;

	std::vector<Mesh*> meshList;
	std::vector<Texture*> diffuseMaps;
	std::vector<Texture*> normalMaps;
	std::vector<unsigned int> meshToTex;

	std::string modelName;

	glm::vec3   translate;  // T
	glm::vec3   rotate;		// R
	glm::vec3   scale;      // S

	Material* material;

	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;
};
class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, AnimModel* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

		if (!scene)
		{
			std::cout << animationPath << " Animation 로드 실패 : " << importer.GetErrorString() << std::endl;
			return;
		}

		assert(scene && scene->mRootNode);
		aiAnimation* animation = scene->mAnimations[0];
		duration = animation->mDuration;
		ticksPerSecond = animation->mTicksPerSecond;
		ReadHierarchyData(rootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}
	~Animation() = default;

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(bones.begin(), bones.end(),
			[&](const Bone* bone)
			{
				return bone->GetBoneName() == name;
			}
		);
		if (iter == bones.end())
			return nullptr;
		else
			return (*iter);
	}

	float GetTicksPerSecond() { return ticksPerSecond; }
	float GetDuration() { return duration; }
	const AssimpNodeData& GetRootNode() { return rootNode; }
	const std::map<std::string, BoneInfo>& GetBoneIDMap() { return boneInfoMap; }

private:
	void ReadMissingBones(const aiAnimation* animation, AnimModel& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();
		int& boneCount = model.GetBoneCount();

		for (int i = 0; i < size; i++)
		{
			aiNodeAnim* channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;
			//size_t pos = boneName.find('_');
			//if (pos != std::string::npos)
			//	boneName = boneName.substr(pos + 1);

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			this->bones.push_back(new Bone(channel->mNodeName.data, boneInfoMap[boneName].id, channel));
		}

		this->boneInfoMap = boneInfoMap;
	}
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		//std::string nodeName = src->mName.data;
		//size_t pos = nodeName.find('_');
		//if (pos != std::string::npos)
		//	nodeName = nodeName.substr(pos + 1);

		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childcount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}

	float duration;
	int ticksPerSecond;
	std::vector<Bone*> bones;
	AssimpNodeData rootNode;
	std::map<std::string, BoneInfo> boneInfoMap;
};
class Animator
{
public:

	Animator(Animation* animation)
	{
		currentTime = 0.f;
		currentAnimation = animation;

		finalBoneMatrices.reserve(MAX_BONE_COUNT);

		for (int i = 0; i < MAX_BONE_COUNT; i++)
		{
			finalBoneMatrices.push_back(glm::mat4(1.f));
		}
	}

	void UpdateAnimation(float deltaTimeIn)
	{
		deltaTime = deltaTimeIn;
		if (currentAnimation)
		{
			currentTime += currentAnimation->GetTicksPerSecond() * deltaTime;
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
			CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.f));
		}
	}
	void PlayAnimation(Animation* pAnimation)
	{
		currentAnimation = pAnimation;
		currentTime = 0.f;
	}
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = currentAnimation->FindBone(nodeName);

		if (bone)
		{
			bone->Update(currentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childcount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return finalBoneMatrices;
	}
	Animation* GetCurrAnimation() { return currentAnimation; }
	void Reset()
	{
		currentAnimation = nullptr;
		currentTime = 0.f;
		fill(finalBoneMatrices.begin(), finalBoneMatrices.end(), glm::mat4(1.f));
	}


private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;
};