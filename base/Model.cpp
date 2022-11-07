#include"Model.h"

#ifdef __ASSIMP_NEED

Model::Model(const char* path)
{
	load_model(path);
}

void Model::draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(renderer);
	}
}

void Model::load_model(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}
	// 处理子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<vertex_t> vertices;
	std::vector<unsigned int> indices;

	// 处理顶点位置
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex_t vertex;
		vector_t vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = -mesh->mVertices[i].z;
		vector.w = 1.0f;
		vertex.pos = vector;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = -mesh->mNormals[i].z;

		vertices.push_back(vertex);
	}

	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(vertices,indices);
}

#endif // __ASSIMP_NEED
