#pragma once
#include "pch.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};
struct SubMesh {
	std::string strName;       // ¸ÓÆ¼¸®¾ó ÀÌ¸§
	std::vector<Vertex> vertices;  // Á¤Á¡ µ¥ÀÌÅÍ
	std::vector<GLuint> indices;  // ÀÎµ¦½º µ¥ÀÌÅÍ
	GLuint texture;             // ÅØ½ºÃ³ ID
	GLuint VAO = 0, VBO = 0, EBO = 0; // ¹öÆÛ °´Ã¼µé

	~SubMesh() {
		if (VAO) glDeleteVertexArrays(1, &VAO);
		if (VBO) glDeleteBuffers(1, &VBO);
		if (EBO) glDeleteBuffers(1, &EBO);
	}
};
struct Group {
	std::string strName;        // ±×·ì ÀÌ¸§
	std::vector<SubMesh> subMeshes;  // ÀÌ ±×·ì¿¡ ¼ÓÇÑ ¼­ºê¸Å½¬µé

	Group(const std::string& name) : strName(name) {}

	void AddSubMesh(const SubMesh& subMesh) {
		subMeshes.push_back(subMesh);
	}
	friend std::ostream& operator<<(std::ostream& os, const Group& group) {
		os << "Group Name: " << group.strName << "\n";
		os << "Number of SubMeshes: " << group.subMeshes.size() << "\n";

		// °¢ ¼­ºê¸Å½¬ Á¤º¸ Ãâ·Â
		for (const auto& subMesh : group.subMeshes) {
			os << "  SubMesh Name: " << subMesh.strName << "\n";
		}
		return os;
	}
};
struct Material {
	std::string strName;  // ¸ÓÆ¼¸®¾ó ÀÌ¸§
	float shininess;						// Shininess
	glm::vec3 ambient;					// Ambient Color
	glm::vec3 diffuse;					// Diffuse Color
	glm::vec3 specular;					// Specular Color
	glm::vec3 emissive;					// Emissive Color
	float Ni;						// Optical Density (Refractive Index)
	float d;						// Transparency
	int illum;						// Illumination model (Illumination Type)
	std::string texture;			// Diffuse Texture (map_Kd)

	Material()
		: ambient(1.0f, 1.0f, 1.0f), diffuse(1.0f, 1.0f, 1.0f), specular(1.0f, 1.0f, 1.0f), shininess(0.0f), d(1.0f), emissive(0.0f, 0.0f, 0.0f), Ni(1.0f), illum(2), texture("") {}
	friend std::ostream& operator<<(std::ostream& os, const Material& material) {
		os << "Material Name: " << material.strName << "\n"
			<< "Shininess: " << material.shininess << "\n"
			<< "Ambient: " << material.ambient.r << ", " << material.ambient.g << ", " << material.ambient.b << "\n"
			<< "Diffuse: " << material.diffuse.r << ", " << material.diffuse.g << ", " << material.diffuse.b << "\n"
			<< "Specular: " << material.specular.r << ", " << material.specular.g << ", " << material.specular.b << "\n"
			<< "Emissive: " << material.emissive.r << ", " << material.emissive.g << ", " << material.emissive.b << "\n"
			<< "Refractive Index (Ni): " << material.Ni << "\n"
			<< "Dissolve (d): " << material.d << "\n"
			<< "Illumination Model: " << material.illum << "\n"
			<< "Texture: " << material.texture << "\n";
		return os;
	}
};

static GLuint LoadTexture(const string& filepath) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // SÃà ¹Ý»ç ·¡ÇÎ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Ãà¼Ò ÇÊÅÍ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cerr << "Failed to load texture: " << filepath << " || ";
		glDeleteTextures(1, &texture); // ÅØ½ºÃ³ »èÁ¦
		return 0; // ¿À·ù ¹ß»ý ½Ã 0 ¹ÝÈ¯
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
static vector<Material> ReadMtl(const string& fileName, const string& folderName) {
	std::vector<Material> materials;
	std::ifstream file("Models/" + folderName +'/' + fileName + ".mtl");
	if (!file.is_open()) {
		std::cerr << "Failed to open MTL file: " << fileName << std::endl;
		return materials;
	}
	std::string line;
	Material currentMaterial;
	bool firstMaterialProcessed = false;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "newmtl") {
			if (firstMaterialProcessed) {
				materials.push_back(currentMaterial);
			}
			else {
				firstMaterialProcessed = true;
			}
			std::string materialName;
			iss >> materialName;
			currentMaterial = Material();
			currentMaterial.strName = materialName;
		}
		else if (prefix == "Ka") {
			iss >> currentMaterial.ambient.r >> currentMaterial.ambient.g >> currentMaterial.ambient.b;
		}
		else if (prefix == "Kd") {
			iss >> currentMaterial.diffuse.r >> currentMaterial.diffuse.g >> currentMaterial.diffuse.b;
		}
		else if (prefix == "Ks") {
			iss >> currentMaterial.specular.r >> currentMaterial.specular.g >> currentMaterial.specular.b;
		}
		else if (prefix == "Ke") {
			iss >> currentMaterial.emissive.r >> currentMaterial.emissive.g >> currentMaterial.emissive.b;
		}
		else if (prefix == "Ni") {
			iss >> currentMaterial.Ni;
		}
		else if (prefix == "d") {
			iss >> currentMaterial.d;
		}
		else if (prefix == "illum") {
			iss >> currentMaterial.illum;
		}
		else if (prefix == "map_Kd") {
			iss >> currentMaterial.texture;
		}
	}
	materials.push_back(currentMaterial);
	file.close();
	return materials;
}
static vector<Group> ReadObj(const string& fileName, const string& folderName, const vector<Material>& materials) {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<Group> groups;
	Group* currentGroup = nullptr;  // ÇöÀç ±×·ì Æ÷ÀÎÅÍ
	SubMesh currentSubMesh;

	std::ifstream file("Models/" + folderName +'/' + fileName + ".obj");
	if (!file.is_open()) {
		//std::cerr << "Failed to open OBJ file: " << fileName << std::endl;
		return groups;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			// Á¤Á¡ ÁÂÇ¥ Ã³¸®
			glm::vec3 pos;
			iss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}
		else if (prefix == "vt") {
			// ÅØ½ºÃ³ ÁÂÇ¥ Ã³¸®
			glm::vec2 vt;
			iss >> vt.x >> vt.y;
			vt.y = 1.0f - vt.y;  // OpenGLÀÇ ÅØ½ºÃ³ ÁÂÇ¥´Â y°¡ ¹ÝÀüµÇ¹Ç·Î ¼öÁ¤
			texCoords.push_back(vt);
		}
		else if (prefix == "vn") {
			// ¹ý¼± º¤ÅÍ Ã³¸®
			glm::vec3 vn;
			iss >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		else if (prefix == "g") {
			std::string groupName;
			iss >> groupName;

			if (currentGroup != nullptr && !currentSubMesh.vertices.empty()) {
				currentGroup->AddSubMesh(currentSubMesh);
			}

			groups.push_back(Group(groupName));
			currentGroup = &groups.back();

			currentSubMesh = SubMesh();
		}
		else if (prefix == "usemtl") {
			std::string materialName;
			iss >> materialName;

			// ¸ÓÆ¼¸®¾ó º¯°æ ½Ã ¼­ºê¸Å½¬¸¦ ÀÌÀü ±×·ì¿¡ Ãß°¡
			if (currentGroup != nullptr && !currentSubMesh.vertices.empty()) {
				currentGroup->AddSubMesh(currentSubMesh);  // ÇöÀç ¼­ºê¸Å½¬¸¦ ±×·ì¿¡ Ãß°¡
			}

			// »õ·Î¿î ¼­ºê¸Å½¬ ½ÃÀÛ
			currentSubMesh = SubMesh();
			currentSubMesh.strName = materialName;

			// ¸ÓÆ¼¸®¾ó¿¡ ÇØ´çÇÏ´Â ÅØ½ºÃ³ ·Îµå
			auto it = std::find_if(materials.begin(), materials.end(),
				[&materialName](const Material& mat) { return mat.strName == materialName; });
			if (it != materials.end()) {
				currentSubMesh.texture = LoadTexture("Models/" + folderName + "/textures/" + it->texture);
				if (currentSubMesh.texture == 0) {
					cout << currentGroup->strName << endl;
				}
			}
		}
		else if (prefix == "f") {
			// ¾ó±¼(Æú¸®°ï) Ã³¸®
			std::string vertexData;
			while (iss >> vertexData) {
				std::istringstream vertexStream(vertexData);
				std::string indexStr;
				int posIdx = 0, texIdx = 0, normIdx = 0;

				// /·Î ±¸ºÐµÈ ÀÎµ¦½º Ã³¸®
				std::getline(vertexStream, indexStr, '/');
				posIdx = std::stoi(indexStr);

				if (std::getline(vertexStream, indexStr, '/')) {
					if (!indexStr.empty()) texIdx = std::stoi(indexStr);
				}

				if (std::getline(vertexStream, indexStr)) {
					if (!indexStr.empty()) normIdx = std::stoi(indexStr);
				}

				Vertex vertex = {};
				vertex.position = positions[posIdx - 1];
				if (texIdx > 0) vertex.texCoord = texCoords[texIdx - 1];
				if (normIdx > 0) vertex.normal = normals[normIdx - 1];
				currentSubMesh.vertices.push_back(vertex);
				currentSubMesh.indices.push_back(currentSubMesh.vertices.size() - 1);  // ÀÎµ¦½º Ãß°¡
			}
		}
	}

	if (currentGroup != nullptr && !currentSubMesh.vertices.empty()) {
		currentGroup->AddSubMesh(currentSubMesh);  // ¸¶Áö¸· ¼­ºê¸Å½¬¸¦ ±×·ì¿¡ Ãß°¡
	}

	file.close();
	return groups;
}
static std::pair<vector<Material>, vector<Group>> ReadModel(const string& fileName, const string& folderName) {
	vector<Material> materials = ReadMtl(fileName, folderName);

	// ¸ÓÆ¼¸®¾ó Ãâ·Â (µð¹ö±ë¿ë)
	for (auto mt : materials)
		std::cout << mt << std::endl;

	std::pair<vector<Material>, vector<Group>> modelPair;
	modelPair.first = materials;
	modelPair.second = ReadObj(fileName, folderName, materials);  // ÀÌÁ¦ GroupÀ» ¹ÝÈ¯
	for (auto gr : modelPair.second)
		std::cout << gr << std::endl;
	return modelPair;
}

class Model {
protected:
	GLenum renderMode;
	mat4 transform = mat4(1.0f);
	vec3 rotationAxis = vec3(0.0f, 1.0f, 0.0f);
	vec3 rotationAtAxis = vec3(0.0f, 1.0f, 0.0f);
	GLfloat rotationAngle = 0.0f;
	GLfloat rotationAtAngle = 0.0f;
	vec3 scale = vec3(1.0f);
	vec3 pivot = vec3(0.0f);
	std::vector<Material> materials;
	std::vector<Group> groups;
	glm::vec3 localAabbMin = glm::vec3(0.0f);
	glm::vec3 localAabbMax = glm::vec3(0.0f);
	void InitBuffer(SubMesh& subMesh);
	const Material* FindMaterial(const std::string& name) const;
public:
	static std::unordered_map<MODEL_TYPE, const std::pair<vector<Material>, vector<Group>>>& modelPairArr;
	vector<vec3> getCrashVetex() {
		vector<vec3> vet{};
		for (Group g : groups) {
			for (const auto& v : g.subMeshes) {
				for (size_t i{}; i < v.vertices.size(); i += 3) {
					vet.push_back(v.vertices[i].position);
				}
			}
		}
	}
	Model(MODEL_TYPE type, GLenum mode);
	void setScale(const vec3& newScale);
	void Render(GLuint shaderProgramID);
	virtual void Update(vec3 position, vec3 rotation, vec3 scale);
	std::pair<glm::vec3, glm::vec3> GetLocalAABB() const { return { localAabbMin, localAabbMax }; }
};
