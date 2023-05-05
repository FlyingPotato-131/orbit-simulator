#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <vec3.h>
#include <vec2.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "model.h"
#include "cube.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const unsigned int windowWidth = 1920;
const unsigned int windowHeight = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// float setmixvalue(GLFWwindow *window, float value){
// 	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && value < 1){
// 		value += 0.01;
// 	}
// 	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && value > 0){
// 		value -= 0.01;
// 	}
// 	return value;
// }

// struct vertex{
// 	vec3 pos;
// 	vec2 texCoords;
// };

int main(){
	camera mainview = createCamera(
		{0.0f, 0.0f, 3.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}
	);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Orbit Simulator", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouseCamera);

    // glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_MULTISAMPLE);

	glViewport(0, 0, windowWidth, windowHeight);
	unsigned int shaderProgram = loadShaders("../resources/shaders/vertex_shader.vert", "../resources/shaders/fragment_shader.frag");
	unsigned int skyboxShader = loadShaders("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");

	// stbi_set_flip_vertically_on_load(true);

	model backpack = loadModel("../resources/models/Buran2/buran.obj");
	// model backpack = loadModel("../resources/models/backpack/backpack.obj");
	std::cout << "Loaded model" << std::endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


	// unsigned int diffMap = loadTexture("../resources/textures/container2.png");
	// unsigned int specMap = loadTexture("../resources/textures/container2_specular.png");

	// std::cout << diffMap << " " << specMap << std::endl;

	glUseProgram(shaderProgram);
	// setInt(shaderProgram, "material.diffuse", 0);
	// setInt(shaderProgram, "material.specular", 1);

	// float mixvalue = 0.5;

	// glm::mat4 model = glm::mat4(1.0f);
	// model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// glm::mat4 view = glm::mat4(1.0f);
	// view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);

	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	double x, y;
    glfwGetCursorPos(window, &x, &y);

    double v;
	glfwSetWindowUserPointer(window, &v);

	glfwSetScrollCallback(window, +[](GLFWwindow * const w, double const x, double const y) noexcept
    {
    	double *scroll = reinterpret_cast<double *>(glfwGetWindowUserPointer(w));
    	*scroll = y;
    });

    // unsigned int lightVAO;
	// glGenVertexArrays(1, &lightVAO);
	// glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container’s VBO’s data
	// already contains the data.
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// // set the vertex attribute
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// glEnableVertexAttribArray(0);

	// setVec3(shaderProgram, "objectColor", 1.0f, 0.5f, 0.31f);
	// setVec3(shaderProgram, "lightColor", 1.0f, 1.0f, 1.0f);

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	setVec3(shaderProgram, "light.position", lightPos);
	setFloat(shaderProgram, "light.constant", 1.0f);
	setFloat(shaderProgram, "light.linear", 0.09f);
	setFloat(shaderProgram, "light.quadratic", 0.032f);

	glEnable(GL_CULL_FACE);

	// stbi_set_flip_vertically_on_load(false);
	stbi_set_flip_vertically_on_load(true);

	std::string skyboxPath = "../resources/textures/StarSkybox04NamedConstell/";

	std::vector<std::string> faces{
		skyboxPath + "StarSkybox044.png",
		skyboxPath + "StarSkybox043.png",
		skyboxPath + "StarSkybox045.png",
		skyboxPath + "StarSkybox046.png",
		skyboxPath + "StarSkybox041.png",
		skyboxPath + "StarSkybox042.png"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// glUseProgram(skyboxShader);

	// unsigned int skyboxVAO;
	// unsigned int skyboxVBO;
	// glGenVertexArrays(1, &skyboxVAO);
	// glGenBuffers(1, &skyboxVBO);

	// glBindVertexArray(skyboxVAO);
	// glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// glBindVertexArray(skyboxVAO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexCoords), &cubeVertexCoords[0], GL_STATIC_DRAW);

	// // vertex coords
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// // vertex normals
	// // glEnableVertexAttribArray(1);
	// // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, norm));

	// // vertex texture coords
	// glEnableVertexAttribArray(2);
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)3);
	// glBindVertexArray(0);

	unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexCoords), &cubeVertexCoords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glUseProgram(skyboxShader);
    setInt(skyboxShader, "skybox", 0);

    // glEnable(GL_PROGRAM_POINT_SIZE);

	while(!glfwWindowShouldClose(window)){
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glBindVertexArray(VAO);

		glUseProgram(shaderProgram);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainview = rotateCamera(window, mainview, deltaTime);	
        mouseCameraOld oldpos = {mainview, float(x), float(y)};
        oldpos = mouseCamera(window, x, y, mainview);
        x = oldpos.x;
        y = oldpos.y;
       	mainview = oldpos.view; 

		// glm::mat4 trans = glm::mat4(1.0f);
		// trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		// unsigned int transformLoc = glGetUniformLocation(shaderProgram,"transform");
		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glm::mat4 model = glm::mat4(1.0f);
		// model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		

		// const float radius = 10.0f;
		// float camX = sin(glfwGetTime()) * radius;
		// float camZ = cos(glfwGetTime()) * radius;
		glm::mat4 view;
		// view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		// view = glm::lookAt(mainview.pos, mainview.pos + mainview.front, mainview.up);
		// view = glm::lookAt(mainview.pos, mainview.target, mainview.up);
		view = glm::lookAt(mainview.target + mainview.distance * cameraPos(mainview), mainview.target, mainview.up);

		setMat4(shaderProgram, "model", model);
		setMat4(shaderProgram, "view", view);
		setMat4(shaderProgram, "projection", projection);

		setVec3(shaderProgram, "viewPos", cameraPos(mainview));

		// setVec3(shaderProgram, "material.ambient", 1.0f, 0.5f, 0.31f);
		// setVec3(shaderProgram, "material.diffuse", 1.0f, 0.5f, 0.31f);
		// setVec3(shaderProgram, "material.specular", 0.5f, 0.5f, 0.5f);

		// setVec3(shaderProgram, "light.ambient", 0.2f, 0.2f, 0.2f);
		// setVec3(shaderProgram, "light.diffuse", 0.5f, 0.5f, 0.5f); // darkened
		setVec3(shaderProgram, "light.specular", 1.0f, 1.0f, 1.0f);

		glm::vec3 lightColor = glm::vec3(1.0f);
		// lightColor.x = sin(glfwGetTime() * 2.0f);
		// lightColor.y = sin(glfwGetTime() * 0.7f);
		// lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		setVec3(shaderProgram, "light.ambient", ambientColor);
		setVec3(shaderProgram, "light.diffuse", diffuseColor);
		// std::cout << diffuseColor.x << std::endl;

		setFloat(shaderProgram, "material.shininess", 32.0f);
		// std::cout << "drawing" << std::endl;
		setMat3(shaderProgram, "normalmtr", glm::inverse(glm::mat3(model)), true);

		draw(shaderProgram, backpack);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
       	glUseProgram(skyboxShader); 

        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        setMat4(skyboxShader, "view", view);
        setMat4(skyboxShader, "projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

		// glDepthMask(GL_FALSE);
		// glUseProgram(skyboxShader);

		// setMat4(skyboxShader, "view", glm::mat4(glm::mat3(view)));
		// setMat4(skyboxShader, "projection", projection);
		
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		// glDepthMask(GL_TRUE);

        // mixvalue = setmixvalue(window, mixvalue);
        // setFloat(shaderProgram, "mixvalue", mixvalue);

		// glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffMap);
        // glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, specMap);

		// glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		// glBindTexture(GL_TEXTURE_2D, texture);

		// glBindVertexArray(VAO);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// for(unsigned int i = 0; i < 10; i++)
		// {
		// 	glm::mat4 model = glm::mat4(1.0f);
		// 	model = glm::translate(model, cubePositions[i]);
		// 	float angle = 20.0f * i;
		// 	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		// 	setMat4(shaderProgram, "model", model);
		//  setMat3(shaderProgram, "normalmtr", glm::inverse(glm::mat3(model)), true);
		// 	glDrawArrays(GL_TRIANGLES, 0, 36);
		// }

		// glBindVertexArray(lightVAO);

		// glUseProgram(lightShader);

		// setVec3(lightShader, "lightColor", lightColor);

		// setMat4(shaderProgram, "model", light);
		// setMat4(shaderProgram, "view", view);
		// setMat4(shaderProgram, "projection", projection);

		// glDrawArrays(GL_TRIANGLES, 0, 36);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    // glDeleteProgram(lightShader);

	glfwTerminate();
	return 0;
}