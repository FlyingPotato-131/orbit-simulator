#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vec3.h>
#include <vector>
#include <fstream>

const int WindowWidth = 1920;
const int WindowHeight = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glewExperimental = GL_TRUE;
	GLenum const glewError = glewInit();

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Orbit Simulator", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WindowWidth, WindowHeight);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	std::vector<vec3> vertices{
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f}
	};

	// float vertices[] = {
	// 	-0.5f, -0.5f, 0.0f,
	// 	0.5f, -0.5f, 0.0f,
	// 	0.0f, 0.5f, 0.0f
	// };

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// glBindBuffer(GL_ARRAY_BUFFER, VBO);

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	std::ifstream vfile("vertex_shader.vert");
	char vertexShaderSource[4096] = {'\0'};
	vfile.read(vertexShaderSource, sizeof(vertexShaderSource));

	glShaderSource(vertexShader, 1, reinterpret_cast<char const * const *>(&vertexShaderSource), NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
		infoLog << std::endl;
	}

	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int fragShader;
	fragShader = glCreateShader(GL_VERTEX_SHADER);

	std::ifstream ffile;
	char fragShaderSource[4096] = {'\0'};
	ffile.read(fragShaderSource, sizeof(fragShaderSource));

	glShaderSource(fragShader, 1, reinterpret_cast<char const * const *>(&fragShaderSource), NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" <<
		infoLog << std::endl;
	}	

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	while(!glfwWindowShouldClose(window)){
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		// 1. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
		(void*)0);
		glEnableVertexAttribArray(0);
		// 2. use our shader program when we want to render an object
		glUseProgram(shaderProgram);
		// 3. now draw the object
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
	glfwTerminate();
}