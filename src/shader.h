#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int loadShaders(const char *vertexPath, const char *fragPath){
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, frag;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
		infoLog << std::endl;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fShaderCode, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" <<
		infoLog << std::endl;
	}

	unsigned int ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, frag);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
		infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(frag);
	return {ID};
}

void setBool(const unsigned int &program, const std::string &name, bool value){
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}
void setInt(const unsigned int &program, const std::string &name, int value){
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void setFloat(const unsigned int &program, const std::string &name, float value){
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void setMat3(const unsigned int &program, const std::string &name, glm::mat3 value, bool transpose = false){
	glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}
void setMat4(const unsigned int &program, const std::string &name, glm::mat4 value, bool transpose = false){
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}
void setVec3(const unsigned int &program, const std::string &name, glm::vec3 value){
	glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(value));
}
void setVec3(const unsigned int &program, const std::string &name, float x, float y, float z){
	setVec3(program, name, {x, y, z});
}

unsigned int loadTexture(const char *path){
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	std::string path_str = path;

	unsigned int texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, path_str.find(".jpg") ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// std::cout << "Loaded texture " << path << std::endl;
	}else{
		std::cout << "Failed to load texture " << path << std::endl;
	}

	stbi_image_free(data);
	return texture;
}

#endif