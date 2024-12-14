#include "pch.h"
#include "Model.h"

std::unordered_map<MODEL_TYPE, const std::vector<vec3>> Model::modelVertices = {
	{MODEL_TYPE::CUBE,   ReadObj("cube.obj")},
	{MODEL_TYPE::SPHERE,   ReadObj("sphere.obj")},
	{MODEL_TYPE::AXIS_MODEL,   ReadObj("axis.obj")},
	{MODEL_TYPE::JETT,   ReadObj("jettSimple.obj")},
	{MODEL_TYPE::PEARL,   ReadObj("Pearl.obj")},
};
std::unordered_map<MODEL_TYPE, const std::string> texturePaths = {
	{MODEL_TYPE::CUBE, "textures/cube_texture.png"},
	{MODEL_TYPE::SPHERE, "textures/sphere_texture.png"},
	{MODEL_TYPE::AXIS_MODEL,   "textures/axis.png"},
	{MODEL_TYPE::JETT,   "TP_Wushu_S0_DF.png"},
	{MODEL_TYPE::PEARL,  "textures/Pearl.png"},
};




 void Model::InitBuffer() {
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

 GLuint Model::LoadTexture(string filepath) {
	 GLuint texture;
	 glGenTextures(1, &texture);
	 glBindTexture(GL_TEXTURE_2D, texture);

	 int width, height, nrChannels;
	 unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	 if (data) {
		 GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		 glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		 glGenerateMipmap(GL_TEXTURE_2D);
	 }
	 else {
		 std::cerr << "texture err" << endl;
	 }
	 stbi_image_free(data);

	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	 return texture;
 }
 GLuint Model::LoadTextureForType(MODEL_TYPE type) {
	 auto it = texturePaths.find(type);
	 if (it != texturePaths.end()) {
		 return LoadTexture(it->second.c_str());
	 }
	 return 0; // 텍스처가 없으면 0 반환
 }

 Model::Model(MODEL_TYPE type, GLenum mode)
	: renderMode(mode), transform(mat4(1.0f)) {
	auto it = modelVertices.find(type);
	if (it != modelVertices.end()) {
		vertices = it->second;
	}
	else {
		throw std::runtime_error("Model type not found in modelVertices.");
	}
	InitBuffer();
}
 Model::~Model() {
	if (VBO) glDeleteBuffers(1, &VBO);
	if (VAO) glDeleteVertexArrays(1, &VAO);
}

 void Model::setRotationAt(float angle, const vec3& axis, const vec3& pivot) {
	rotationAtAngle = angle;
	rotationAtAxis = axis;
	this->pivot = pivot;
}
 void Model::setPosition(const vec3& newPosition) {
}
 void Model::setRotation(float angle, const vec3& axis) {
	rotationAngle = angle;
	rotationAxis = axis;
}
 void Model::setScale(const vec3& newScale) {
	 scale = newScale;
 }

 void Model::editVertex(const std::function<void(std::vector<vec3>&)>& func) {
	func(vertices);
	InitBuffer();
}
 void Model::Render(GLuint& shaderProgram) {
	 glBindVertexArray(VAO);

	 GLuint modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	 glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(transform));

	 // 텍스처 활성화
	 glActiveTexture(GL_TEXTURE0);
	 glBindTexture(GL_TEXTURE_2D, textureID);
	 GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSampler");
	 glUniform1i(textureLocation, 0);

	 glDrawArrays(renderMode, 0, static_cast<GLsizei>(vertices.size() / 2));
	 glBindVertexArray(0);
 }
 void Model::Update(vec3 position, vec3 rotation, vec3 scale) {
	 mat4 toPivot = glm::translate(mat4(1.0f), -pivot);
	 mat4 rotationAtPivot = glm::rotate(mat4(1.0f), glm::radians(rotationAtAngle), rotationAtAxis);
	 mat4 fromPivot = glm::translate(mat4(1.0f), pivot);
	 mat4 translation = glm::translate(mat4(1.0f), position);

	 // 각 축에 대한 회전
	 mat4 rotationX = glm::rotate(mat4(1.0f), glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	 mat4 rotationY = glm::rotate(mat4(1.0f), glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	 mat4 rotationZ = glm::rotate(mat4(1.0f), glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	 mat4 rotationMatrix = rotationX * rotationY * rotationZ;

	 mat4 scaling = glm::scale(mat4(1.0f), scale);

	 transform = fromPivot * rotationAtPivot * toPivot * translation * rotationMatrix * scaling;
 }
