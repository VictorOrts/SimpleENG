#ifndef _H_MESH_
#define _H_MESH_

#include <string>
#include <vector>

class Mesh{
public:
	Mesh();
	~Mesh();
	 void Load(std::string obj);
	 unsigned int vao, evo, nindex;
	 void createquad();
protected:
	
	std::vector<float> vertex;
	std::vector<float> normals;
	std::vector<float> uv;
	std::vector<unsigned int> index;
};

#endif // !_H_MESH_