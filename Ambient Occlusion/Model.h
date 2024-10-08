#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include "stb_image.h"

#include <string>
#include <vector>
#include <iostream>

#include "UtilityFunctions.h"
#include "Mesh.h"
#include "Shader.h"

using std::cout;
using std::endl;
using std::string;

class Model
{
public:
	vector<Texture> texturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh>    meshes;
	string directory;

	Model(const string &path);
	void Draw(Shader& shader);

private:
	void _LoadModel(const string &path);
	void _ProcessNode(aiNode* node, const aiScene* scene);
	Mesh _ProcessMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> _LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int _TextureFromFile(const char* path, const string& directory);
};

class ModelList
{
public:
	vector<Model> models;
	vector<glm::mat4> matrix;
};