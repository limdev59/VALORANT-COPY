#include "pch.h"
#include "ShaderMgr.h"

ShaderMgr::ShaderMgr()
{
}
ShaderMgr::~ShaderMgr() {
}

string ShaderMgr::fileToBuf(const string& filename) {
    std::ifstream in{ filename };
    if (!in) {
        std::cerr << filename << " 파일 존재하지 않음!" << endl;
        exit(5);
    }
    string str, temp;
    while (std::getline(in, temp)) {
        str += temp;
        str.append(1, '\n');
    }
    return str;
}
GLuint ShaderMgr::compileShader(const string& source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        GLchar errorLog[512];
        glGetShaderInfoLog(shader, 512, NULL, errorLog);
        std::cerr << "ERROR: Shader 컴파일 실패\n" << errorLog << endl;
        exit(1);
    }
    return shader;
}
GLuint ShaderMgr::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        GLchar errorLog[512];
        glGetProgramInfoLog(program, 512, NULL, errorLog);
        std::cerr << "ERROR: Shader program 링크 실패\n" << errorLog << endl;
        exit(1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
GLuint ShaderMgr::createShaderProgram(const string& name, const string& vertexPath, const string& fragmentPath) {
    string vertexSourceStr = fileToBuf(vertexPath);
    const char* vertexSource = vertexSourceStr.c_str();
    string fragmentSourceStr = fileToBuf(fragmentPath);
    const char* fragmentSource = fragmentSourceStr.c_str();

    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = linkProgram(vertexShader, fragmentShader);

    shaderPrograms[name] = shaderProgram;
    return shaderProgram;
}
void ShaderMgr::cleanUp() {
    for (auto& pair : shaderPrograms)
        glDeleteProgram(pair.second);

    shaderPrograms.clear();
}
void ShaderMgr::Init()
{
    GLuint a = createShaderProgram("default", "vertex.glsl", "fragment.glsl");
}
GLuint ShaderMgr::getShaderProgram(const string& name) {
    if (shaderPrograms.find(name) != shaderPrograms.end()) {
        return shaderPrograms[name];
    }
    else {
        std::cerr << "ERROR: 요청한 쉐이더 프로그램 " << name << "이 존재하지 않음!" << endl;
        exit(2);
    }
}
