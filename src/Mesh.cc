#include "Mesh.h"

#include "OpenGLFun.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tinyobjloaderlegacy.h"


Mesh::Mesh() {
	vao = 0;
	evo = 0;
	nindex = 0;
}

Mesh::~Mesh() {
	printf("Deleted");
}

void Mesh::Load(std::string obj) {
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, obj.c_str());
	//printf("\n%s", name_.c_str());
	for (unsigned int i = 0; i < shapes.size(); i++)
	{
		for (unsigned int j = 0; j < shapes[i].mesh.positions.size(); j++)
		{
			vertex.push_back(shapes[i].mesh.positions[j]);
			normals.push_back(shapes[i].mesh.normals[j]);
		}

		for (unsigned int j = 0; j < shapes[i].mesh.texcoords.size(); j++)
		{
			uv.push_back(shapes[i].mesh.texcoords[j]);
		}
		for (unsigned int j = 0; j < shapes[i].mesh.indices.size(); j++)
		{
			index.push_back(shapes[i].mesh.indices[j]);
		}
		//std::size_t pos = obj.find_last_of("/");
		//name_ = name_.substr(pos + 1);
		//printf("\n%s", name_.c_str());
		nindex = index.size();
	}


	OpenGL::LoadGeometry(&vao, &evo, &nindex, vertex, normals, uv, index);

	printf("\nVao %d Evo %d Index %d", vao, evo, nindex);
}

void Mesh::createquad() {

		float vertexData[12]{
			-1.0, -1.0, 1.0,
			1.0, -1.0, 1.0,
			1.0, 1.0, 1.0,
			-1.0, 1.0, 1.0
		};

		float uvData[8]{
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};
		unsigned int indices[6] = {
			0, 1, 2, 0, 2, 3, // front
		};

		std::vector<float> vertex;
		std::vector<float> uv;
		std::vector<unsigned int> index;

		for (int i = 0; i < 12; i++)
		{
			vertex.push_back(vertexData[i]);
		}
		for (int i = 0; i < 8; i++)
		{
			uv.push_back(uvData[i]);
		}
		for (int i = 0; i < 6; i++)
		{
			index.push_back(indices[i]);
		}
		//LOAD QUAD
		OpenGL::LoadGeometry(&vao, &evo, &nindex, vertex,normals, uv, index);
		nindex = index.size();
		printf("\nVao %d Evo %d Index %d", vao, evo, nindex);
}