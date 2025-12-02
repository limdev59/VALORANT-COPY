#include "pch.h"
#include "Model.h"
#include <cfloat>

void Model::InitBuffer(SubMesh& subMesh) {
	glGenVertexArrays(1, &subMesh.VAO);
	glGenBuffers(1, &subMesh.VBO);
	glGenBuffers(1, &subMesh.EBO);

	glBindVertexArray(subMesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, subMesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, subMesh.vertices.size() * sizeof(Vertex), subMesh.vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh.indices.size() * sizeof(GLuint), subMesh.indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

const Material* Model::FindMaterial(const std::string& name) const {
	for (const auto& material : materials) {
		if (material.strName == name) {
			return &material;
		}
	}
	return nullptr; // 이름에 해당하는 Material을 찾지 못한 경우
}

Model::Model(MODEL_TYPE type, GLenum mode)
	: renderMode(mode), transform(mat4(1.0f)) {
	materials = modelPairArr[type].first;
	cout << modelPairArr[type].second.size()<<endl;
	groups = modelPairArr[type].second;
	for (auto& material : materials) {
		Group group(material.strName);
		for (auto& groupItem : groups) {
			for (auto& subMesh : groupItem.subMeshes) {
				if (subMesh.strName == material.strName) {
					group.AddSubMesh(subMesh);
				}
			}
		}
		if (!group.subMeshes.empty()) {
			groups.push_back(group);
		}
	}
	for (auto& group : groups) {
		for (auto& subMesh : group.subMeshes) {
			InitBuffer(subMesh);
		}
	}

        ComputeBoundingBox();
}

void Model::setScale(const vec3& newScale) {
	scale = newScale;
}

void Model::Render(GLuint shaderProgramID) {
	glUseProgram(shaderProgramID);
	for (const auto& group : groups) {
		for (const auto& subMesh : group.subMeshes) {
			glBindVertexArray(subMesh.VAO);

			GLuint modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

			if (subMesh.texture != 0) { // 텍스처가 존재하는 경우
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, subMesh.texture);
				glUniform1i(glGetUniformLocation(shaderProgramID, "diffuseTexture"), 0);
			}

			// 재질 속성 전달
			const Material* material = FindMaterial(subMesh.strName);
			if (material) {
				glUniform3fv(glGetUniformLocation(shaderProgramID, "Ka"), 1, glm::value_ptr(material->ambient));
				glUniform3fv(glGetUniformLocation(shaderProgramID, "Kd"), 1, glm::value_ptr(material->diffuse));
				glUniform3fv(glGetUniformLocation(shaderProgramID, "Ks"), 1, glm::value_ptr(material->specular));
				glUniform1f(glGetUniformLocation(shaderProgramID, "Ns"), material->shininess);
			}


void Model::ComputeBoundingBox() {
        bool initialized = false;
        vec3 minBounds(0.0f);
        vec3 maxBounds(0.0f);

        for (const auto& group : groups) {
                for (const auto& subMesh : group.subMeshes) {
                        for (const auto& vertex : subMesh.vertices) {
                                if (!initialized) {
                                        minBounds = maxBounds = vertex.position;
                                        initialized = true;
                                }
                                else {
                                        minBounds = glm::min(minBounds, vertex.position);
                                        maxBounds = glm::max(maxBounds, vertex.position);
                                }
                        }
                }
        }

        if (initialized) {
                boundingMin = minBounds;
                boundingMax = maxBounds;
        }
        else {
                boundingMin = vec3(-0.5f);
                boundingMax = vec3(0.5f);
        }
}

void Model::GetWorldAABB(vec3& outCenter, vec3& outSize) const {
        vec3 localMin = boundingMin;
        vec3 localMax = boundingMax;

        vec3 corners[8] = {
                {localMin.x, localMin.y, localMin.z},
                {localMax.x, localMin.y, localMin.z},
                {localMin.x, localMax.y, localMin.z},
                {localMax.x, localMax.y, localMin.z},
                {localMin.x, localMin.y, localMax.z},
                {localMax.x, localMin.y, localMax.z},
                {localMin.x, localMax.y, localMax.z},
                {localMax.x, localMax.y, localMax.z}
        };

        vec3 worldMin(FLT_MAX);
        vec3 worldMax(-FLT_MAX);

        for (const auto& corner : corners) {
                vec4 transformed = transform * vec4(corner, 1.0f);
                worldMin = glm::min(worldMin, vec3(transformed));
                worldMax = glm::max(worldMax, vec3(transformed));
        }

        outCenter = (worldMin + worldMax) * 0.5f;
        outSize = worldMax - worldMin;
}
			// 드로우 호출
			glDrawElements(renderMode, subMesh.indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void Model::Update(vec3 position, vec3 rotation, vec3 scale) {
	mat4 toPivot = glm::translate(mat4(1.0f), -pivot);
	mat4 rotationAtPivot = glm::rotate(mat4(1.0f), glm::radians(rotationAtAngle), rotationAtAxis);
	mat4 fromPivot = glm::translate(mat4(1.0f), pivot);
	mat4 translation = glm::translate(mat4(1.0f), position);

	mat4 rotationX = glm::rotate(mat4(1.0f), glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotationY = glm::rotate(mat4(1.0f), glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	mat4 rotationZ = glm::rotate(mat4(1.0f), glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	mat4 rotationMatrix = rotationX * rotationY * rotationZ;

	mat4 scaling = glm::scale(mat4(1.0f), scale);

	transform = fromPivot * rotationAtPivot * toPivot * translation * rotationMatrix * scaling;
}

