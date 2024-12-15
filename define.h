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
	AXIS_MODEL,
	SQUARE,
	JETT,
	PEARL,
	END,
};

static std::vector<glm::vec3> ReadObj(const std::string& filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << " file read failed\n";
		exit(1);
	}

	std::vector<glm::vec3> vertex;
	std::vector<glm::ivec3> index;
	std::vector<glm::ivec2> lineIndex;

	while (in) {
		std::string line;
		std::getline(in, line);
		std::stringstream ss{ line };
		std::string str;
		ss >> str;

		if (str == "v") {
			glm::vec3 v;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				v[i] = std::stof(subStr);
			}
			vertex.push_back(v);
		}
		else if (str == "f") {
			glm::ivec3 f;
			for (int i = 0; i < 3; ++i) {
				std::string substr;
				ss >> substr;
				std::stringstream subss{ substr };
				std::string vIdx;
				std::getline(subss, vIdx, '/');
				f[i] = std::stoi(vIdx) - 1;
			}
			index.push_back(f);
		}
		else if (str == "l") {
			glm::ivec2 l;
			for (int i = 0; i < 2; ++i) {
				std::string substr;
				ss >> substr;
				l[i] = std::stoi(substr) - 1;
			}
			lineIndex.push_back(l);
		}
	}

	std::vector<glm::vec3> data;
	for (auto& f : index) {
		data.push_back(vertex[f[0]]);
		data.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
		data.push_back(vertex[f[1]]);
		data.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
		data.push_back(vertex[f[2]]);
		data.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	}

	for (auto& l : lineIndex) {
		data.push_back(vertex[l[0]]);
		data.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
		data.push_back(vertex[l[1]]);
		data.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
	}

	std::cout << filename << " File Read, " << data.size() / 2 << " Vertices Exists." << std::endl;
	return data;
}
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


