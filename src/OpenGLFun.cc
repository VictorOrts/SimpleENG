#include "OpenGLFun.h"


#include "deps/GL/glew.h"

#include <ostream>
#include <fstream>
#include <sstream>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "external\stb_image.h"

unsigned int OpenGL::CreateTexture() {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	
	return texture;
	
}

std::string OpenGL::ReadFromFile(std::string file) {
	std::ifstream f;
	f.open(file.c_str(), std::ios::in | std::ios::binary);
	//printf("\nShaderFromFile loaded ->%s\n", filePath.c_str());
	//	printf("\nF %s",f.get());
	if (!f.is_open()) {
		
		return NULL;
	}
	else {
		//read whole file into stringstream buffer
		std::stringstream buffer;
		buffer << f.rdbuf();
		std::string info = buffer.str();
		f.close();
		//printf("%s\n", info.c_str());
		return info;
	}

}

unsigned int OpenGL::CreateShader(std::string vertex, std::string fragment) {
	printf("\n%s -- %s", vertex.c_str(), fragment.c_str());
	std::string vcode = ReadFromFile(vertex).c_str();
	std::string fcode = ReadFromFile(fragment).c_str();
	const char* vertexShaderSource = vcode.c_str();
	const char* fragmentShaderSource = fcode.c_str();

	GLint success;
	char infoLog[512];
	int size = vcode.size();
	std::string tmp2;
	//CompileVertex
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexShaderSource, &size);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		tmp2 = infoLog;
		printf("\n%s", tmp2.c_str());
	
	}

	// Fragment shader

	size = fcode.size();
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderSource, &size);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		tmp2.clear();
		tmp2 = infoLog;
		printf("\n%s", tmp2.c_str());

	}

	// Create Program and Link shaders
	GLuint program_ = glCreateProgram();
	glAttachShader(program_, vertexShader);
	glAttachShader(program_, fragmentShader);
	glLinkProgram(program_);
	// Check for linking errors
	glGetProgramiv(program_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program_, 512, NULL, infoLog);
		tmp2.clear();
		tmp2 = infoLog;
		printf("\n%s", tmp2.c_str());
	}
	//Delete Shaders and Clear string
	vcode.clear();
	fcode.clear();
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	printf("\nProgram %d", program_);
	return program_;
}
void OpenGL::LoadGeometry(unsigned int *VAO, unsigned int *EVO, unsigned int *m_indi, std::vector<float> vertex, std::vector<float>normals, std::vector<float> uv, std::vector<unsigned int> index) {
	unsigned int posVBO;
	unsigned int norVBO;
	unsigned int uvVBO;

	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex.size(), &(*vertex.begin()), GL_STATIC_DRAW);
	if (!normals.empty()) {
		glGenBuffers(1, &norVBO);
		glBindBuffer(GL_ARRAY_BUFFER, norVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normals.size(), &(*normals.begin()), GL_STATIC_DRAW);
	}
	else {
		//printf("\nNormales vacias");
	}

	glGenBuffers(1, &uvVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*uv.size(), &(*uv.begin()), GL_STATIC_DRAW);

	glGenBuffers(1, EVO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EVO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*index.size(), &(*index.begin()), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	if (!normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, norVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else {
		//	printf("\nNormales vacias");
	}

	//Unbind all
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	}
void OpenGL::setUniform(const std::string &name, unsigned int program, const glm::mat4 matrix) {
		int posicion = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(posicion, 1, GL_FALSE, &matrix[0][0]);
	}
	void OpenGL::setUniform(const std::string &name, unsigned int program, const glm::vec3 vec3_) {
		int posicion = glGetUniformLocation(program, name.c_str());
		glUniform3f(posicion, vec3_.x, vec3_.y, vec3_.z);
	}
	void OpenGL::setUniform(const std::string &name, unsigned int program, const glm::vec4 vec4_) {
		int posicion = glGetUniformLocation(program, name.c_str());
		glUniform4f(posicion, vec4_.x, vec4_.y, vec4_.z, vec4_.w);
	}
	void OpenGL::setUniform(const std::string &name, unsigned int program, int value)
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void OpenGL::setUniform(const std::string &name, unsigned int program, float value)
	{
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);
	}

	unsigned int OpenGL::LoadTexture(std::string name) {
		int w, h, type;
		unsigned int texture;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* image_ = stbi_load(name.c_str(), &w, &h, &type, 0);
		if (!image_) throw std::runtime_error(stbi_failure_reason());
	
		else {
			
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (type == 3)
			{
				printf("\nRGB");
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				printf("\nRGBA");
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		return texture;
	}

	unsigned int OpenGL::CreateGBufferTexture(int tipo, int w, int h) {
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		if (tipo == 0) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		}
		else if (tipo == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture, 0);
		}
		else if (tipo == 2) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texture, 0);
		}
		return texture;
	}