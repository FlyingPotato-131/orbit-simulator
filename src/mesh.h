#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "shader.h"

struct vertex{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 tex;
	glm::vec3 tan;
};

struct texture{
	unsigned int id;
	// bool type; //0 for diffuse, 1 for specular
	std::string type;
	std::string path;
};

struct mesh{
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture> textures;

	unsigned int VAO, VBO, EBO;
};

unsigned int *setupMesh(std::vector<vertex> vertices, std::vector<unsigned int> indices){
	// std::cout << "setup" << std::endl;
	unsigned int *buffers = static_cast<unsigned int *>(std::malloc(3 * sizeof(unsigned int)));
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, norm));

	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex));

	//vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tan));
	glBindVertexArray(0);
	
	// std::cout << "fin" << std::endl;
	buffers[0] = VAO;
	buffers[1] = VBO;
	buffers[2] = EBO;
	return buffers;
}

mesh createMesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures){
	mesh result;

	result.vertices = vertices;
	result.indices = indices;
	result.textures = textures;

	unsigned int *buffers = setupMesh(vertices, indices);
	result.VAO = buffers[0];
	result.VBO = buffers[1];
	result.EBO = buffers[2];

	return result;
}

void draw(const unsigned int &shader, mesh &object){
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	// std::cout << object.textures.size() << std::endl;
	for(unsigned int i = 0; i < object.textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		// string name = object.textures[i].type;
		if(object.textures[i].type == "diffuse")
			number = std::to_string(diffuseNr++);
		else if(object.textures[i].type == "surface")
			number = std::to_string(specularNr++);
		else
			number = std::string("");

		glBindTexture(GL_TEXTURE_2D, object.textures[i].id);
		// glUseProgram(shader);
		glBindVertexArray(object.VAO);
		// setInt(shader, ("material." + std::string(!object.textures[i].type ? "diffuse" : "surface") + number).c_str(), object.textures[i].id - 1);
		// setInt(shader, ("material." + std::string(!object.textures[i].type ? "diffuse" : "surface") + number).c_str(), i);
		setInt(shader, ("material." + object.textures[i].type + number).c_str(), i);
		// setFloat(shader, "specular1", object.textures[i].id);
		// std::cout << i << std::endl;
		// std::cout << ("material." + std::string(!object.textures[i].type ? "diffuse" : "specular") + number).c_str() << std::endl;
	}

	// glActiveTexture(GL_TEXTURE0 + object.textures.size());
	// glBindTexture(GL_TEXTURE_2D, normalMap);
	// glBindVertexArray(object.VAO);
	// setInt(shader, "material.normalMap", object.textures.size());
	// setInt(shader, "material.normalMap", 2);

	// glActiveTexture(GL_TEXTURE1);

	// draw mesh
	glBindVertexArray(object.VAO);
	glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_INT, 0);
	// std::cout << object.indices.size() << std::endl;
	// std::cout << "drew" << std::endl;
	glBindVertexArray(0);
}