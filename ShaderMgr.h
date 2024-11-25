#pragma once
class ShaderMgr {
    SINGLE(ShaderMgr)
private:
    std::unordered_map<std::string, GLuint> shaderPrograms;
    string fileToBuf(const string& filename);
    GLuint compileShader(const string& source, GLenum shaderType);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    GLuint createShaderProgram(const string& name, const string& vertexPath, const string& fragmentPath);
    void cleanUp();
public:
    void Init();
    GLuint getShaderProgram(const string& name);
};
