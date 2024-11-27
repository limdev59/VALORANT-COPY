#pragma once
class Model {
protected:
	GLenum renderMode;
	GLuint VAO = 0, VBO = 0, textureID = 0;
	mat4 transform = mat4(1.0f);

	vec3 rotationAxis = vec3(0.0f, 1.0f, 0.0f);
	vec3 rotationAtAxis = vec3(0.0f, 1.0f, 0.0f);
	GLfloat rotationAngle = 0.0f;
	GLfloat rotationAtAngle = 0.0f;
	vec3 scale = vec3(1.0f);
	vec3 pivot = vec3(0.0f);  // ÇÇ¹þ Ãß°¡
	vector<vec3> vertices;

	void	InitBuffer();
	GLuint	LoadTexture(string filepath);
	GLuint	LoadTextureForType(MODEL_TYPE type);
public:
	static std::unordered_map<MODEL_TYPE, const vector<vec3>> modelVertices;
	static std::unordered_map<MODEL_TYPE, const vector<GLuint>> modelTextures;

	Model(MODEL_TYPE type, GLenum mode = GL_TRIANGLES);
	virtual ~Model();
public:
	void setRotationAt(float angle, const vec3& axis, const vec3& pivot);
	void setPosition(const vec3& newPosition);
	void setRotation(float angle, const vec3& axis);
	void setScale(const vec3& newScale);

	void editVertex(const std::function<void(std::vector<vec3>&)>& func);
	virtual void Render(GLuint& shaderProgram);
	virtual void Update(vec3 position, vec3 rotation, vec3 scale);

};