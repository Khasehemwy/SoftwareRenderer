#include"Mesh.h"
#ifdef __ASSIMP_NEED


Mesh::Mesh(std::vector<vertex_t> vertices, std::vector<UINT32> indices):
	vertices(vertices),
	indices(indices)
{
}

Mesh::~Mesh()
{
}

void Mesh::draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < indices.size(); i+=3) {
		assert(i + 2 < indices.size());
		vertex_t v1 = vertices[indices[i]];
		vertex_t v2 = vertices[indices[i + 1]];
		vertex_t v3 = vertices[indices[i + 2]];
		renderer.display_primitive(v1, v2, v3);
	}
}

#endif // __ASSIMP_NEED