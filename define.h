#pragma once
#define SINGLE(type)                \
public:                             \
    static type* Instance()         \
    {                               \
        static type instance;       \
        return &instance;           \
    }                               \
private:                            \
    type();                         \
    ~type();                        \

#define fDT TimeMgr::Instance()->GetfDT()
#define DT TimeMgr::Instance()->GetDT()

constexpr GLint	WINDOW_WIDTH = 1200;
constexpr GLint	WINDOW_HEIGHT = 768;
constexpr int	MAX_BONE_INFLUENCE = 4;
constexpr int	MAX_BONE_COUNT = 130;
constexpr int	MAXFPS = 100;

enum class GROUP_TYPE {
	DEFAULT,
	PLAYER,
	ENEMY,
	//MISSILE,
	//MONSTER,
	END,
};
enum class SCENE_TYPE {
	TOOL,
	START,
	STAGE_1,
	STAGE_2,

	END
};
enum class PROJ_TYPE {
	PERSPECTIVE,
	ORTHOGRAPHIC
};
enum class MODEL_TYPE {
	CUBE,
	SPHERE,
	JETT,
	AXIS_MODEL,
	SQUARE,
	PEARL,
	ASCENT,
	END,
};

enum class ANIM_MODEL_TYPE {
	JETT,
	END,
}; 

static string fileToBuf(string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << "파일 존재하지 않음!" << std::endl;
		exit(1);
	}

	string str, temp;
	while (std::getline(in, temp)) {
		str += temp;
		str.append(1, '\n');
	}

	return str;
};
static void make_vertexShaders(GLuint& vertexShader, const string& vertexName)
{
	std::string vertexSourceStr = fileToBuf(vertexName.c_str());
	const char* vertexSource = vertexSourceStr.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
static void make_fragmentShaders(GLuint& fragmentShader, const string& fragmentName)
{
	string fragmentSourceStr = fileToBuf(fragmentName);
	const char* fragmentSource = fragmentSourceStr.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
static void make_shaderProgram(GLuint& shaderProgramID, GLuint& vertexShader, GLuint& fragmentShader , const string& vertexName, const string& fragmentName)
{
	make_vertexShaders(vertexShader, vertexName);
	make_fragmentShaders(fragmentShader, fragmentName);

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	// 링크 성공 여부 확인 코드 추가
	GLint isLinked = 0;
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		GLchar errorLog[512];
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: Shader program 링크 실패\n" << errorLog << std::endl;
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgramID);
}



