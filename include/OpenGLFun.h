#ifndef _H_OPENGL_
#define _H_OPENGL_
#include <string>
#include <deps/glm/glm.hpp>
#include <vector>

namespace OpenGL{

	unsigned int CreateTexture();
	std::string ReadFromFile(std::string file);
	unsigned int CreateShader(std::string vertex, std::string fragment);
	void LoadGeometry(unsigned int *VAO, unsigned int *EVO, unsigned int *m_indi, std::vector<float> vertex, std::vector<float>normals, std::vector<float> uv, std::vector<unsigned int> index);
	void setUniform(const std::string &name, unsigned int program, const glm::mat4 mat4_);
	void setUniform(const std::string &name, unsigned int program, const glm::vec3 vec3_);
	void setUniform(const std::string &name, unsigned int program, const glm::vec4 vec4_);
	void setUniform(const std::string &name, unsigned int program, int value);
	void setUniform(const std::string &name, unsigned int program, float value);
	unsigned int LoadTexture(std::string name);
	unsigned int CreateGBufferTexture(int tipo,int w, int h);
}

#endif // !_H_OPENGL_