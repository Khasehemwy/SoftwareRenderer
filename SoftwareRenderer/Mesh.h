#pragma once
#include"Includes.h"

class Mesh
{
public:
	std::vector<vertex_t> vertices;
	std::vector<UINT32> indices;

	Mesh(std::vector<vertex_t> vertices,std::vector<UINT32> indices);
	~Mesh();

	void draw(Renderer& renderer);

private:

};

