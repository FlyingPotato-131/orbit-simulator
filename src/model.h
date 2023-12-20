#pragma once
#include "mesh.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"

std::vector<texture> textures_loaded; //all the loaded textures so as not to load them for each mesh seperately

struct model{
	std::vector<mesh> meshes;
	std::string directory;
};

void draw(const unsigned int &shader, model &object){
	// std::cout << object.meshes.size() << std::endl;
	for(unsigned int i = 0; i < object.meshes.size() ; i++)
		draw(shader, object.meshes[i]);
}

std::vector<texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string path){
	std::vector<texture> textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
		aiString str;
		// std::cout << "test" << std::endl;
		mat->GetTexture(type, i, &str);
		// std::cout << str.C_Str() << std::endl;
		bool skip = false;
		for(unsigned int j = 0; j < textures_loaded.size(); j++){
			// std::cout << textures_loaded[j].path.data() << " " << str.C_Str() << std::endl;
			if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0){
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if(!skip){
			texture tex;
			// std::cout << "test1" << std::endl;
			tex.id = loadTexture((path + '/' + str.C_Str()).data());
			// std::cout << "test2" << std::endl;
			tex.type = typeName;
			tex.path = str.C_Str();
			textures_loaded.push_back(tex);
			textures.push_back(tex);
			std::cout << "Loaded texture " << str.C_Str() << std::endl;
			// stbi_set_flip_vertically_on_load(true);
		}
	}
	return textures;
}

mesh processMesh(aiMesh *mesh, const aiScene *scene, std::string path){
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture> textures;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++){
		vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;

		glm::vec3 normal;

		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.norm = normal;
		// std::cout << normal.x << std::endl;


		if(mesh->mTextureCoords[0]){ //check if mesh contains texture coords
			glm::vec2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.tex = tex;
		}else
			vertex.tex = glm::vec2(0.0f, 0.0f);

		glm::vec3 tangent;

		tangent.x = mesh->mTangents[i].x;
		tangent.y = mesh->mTangents[i].y;
		tangent.z = mesh->mTangents[i].z;
		vertex.tan = tangent;
		// std::cout << tangent.x << std::endl;

		vertices.push_back(vertex);
	}

	// process indices
	for(unsigned int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		indices.push_back(face.mIndices[j]);
	}

	// process material
	if(mesh->mMaterialIndex >= 0){
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, std::string("diffuse"), path);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// std::cout << diffuseMaps.size() << std::endl;

		std::vector<texture> specularMaps = loadMaterialTextures(material, aiTextureType_SHININESS, std::string("surface"), path);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, std::string("normal"), path);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<texture> lightmaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, std::string("lightmap"), path);
		textures.insert(textures.end(), lightmaps.begin(), lightmaps.end());

		std::vector<texture> emission = loadMaterialTextures(material, aiTextureType_EMISSIVE, std::string("emission"), path);
		textures.insert(textures.end(), emission.begin(), emission.end());
	}

	return createMesh(vertices, indices, textures);
}

std::vector<mesh> processNode(aiNode *node, const aiScene *scene, std::string path){
	std::vector<mesh> meshes;

	// process all the node’s meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, path));
	}

	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		// std::cout << node->mNumChildren << std::endl;
		std::vector<mesh> childMeshes = processNode(node->mChildren[i], scene, path);
		for (int i = 0; i < childMeshes.size(); ++i){
			meshes.push_back(childMeshes[i]);
		}
	}
	// std::cout << meshes.size() << std::endl;

	return meshes;
}

model loadModel(std::string path){
	model result;

	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return result;
	}

	result.directory = path.substr(0, path.find_last_of('/'));
	result.meshes = processNode(scene->mRootNode, scene, result.directory);
	// std::cout << result.meshes.size() << std::endl;

	return result;
}