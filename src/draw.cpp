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
#include "physics.h"
// #include "tree.h"
#include <ncurses.h>
#include <cstdlib>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const unsigned int windowWidth = 1920;
const unsigned int windowHeight = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

int main(){
	camera mainview = createCamera(
		{0.0f, 0.0f, 3.0f},
		{7000.0f, 0.0f, 0.0f},
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_MULTISAMPLE);

	glViewport(0, 0, windowWidth, windowHeight);
	unsigned int shaderProgram = loadShaders("../resources/shaders/vertex_shader.vert", "../resources/shaders/fragment_shader.frag");
	unsigned int skyboxShader = loadShaders("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");

	// stbi_set_flip_vertically_on_load(true);

	model buran = loadModel("../resources/models/Buran/scene.gltf");
	std::cout << "Loaded model" << std::endl;

	unsigned int normalMap = loadTexture("../resources/models/Buran/textures/buran_Color_normal.png");

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glUseProgram(shaderProgram);

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

	glm::vec3 lightPos(1.2f, 0.0f, -2.0f);

	setVec3(shaderProgram, "light.position", lightPos);
	setFloat(shaderProgram, "light.constant", 1.0f);
	setFloat(shaderProgram, "light.linear", 0.09f);
	setFloat(shaderProgram, "light.quadratic", 0.032f);

	// glBindVertexArray(backpack.VAO);
	// setInt(shaderProgram, "material.normalMap", normalMap);

	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 diffuseColor = lightColor * glm::vec3(2.5f);
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.025f);
	setVec3(shaderProgram, "light.ambient", ambientColor);
	setVec3(shaderProgram, "light.diffuse", diffuseColor);

	// stbi_set_flip_vertically_on_load(false);
	stbi_set_flip_vertically_on_load(true);

	std::string skyboxPath = "../resources/textures/StarSkybox04/";

	std::vector<std::string> faces{
		skyboxPath + "StarSkybox044.png",
		skyboxPath + "StarSkybox043.png",
		skyboxPath + "StarSkybox045.png",
		skyboxPath + "StarSkybox046.png",
		skyboxPath + "StarSkybox041.png",
		skyboxPath + "StarSkybox042.png"
	};
	// unsigned int cubemapTexture = loadCubemap(faces);
	//main env textures
	unsigned int starsTexture = loadTexture("../resources/textures/hiptyc_2020_4k.jpg");
	unsigned int earthDay = loadTexture("../resources/textures/2k_earth_daymap.jpg");
	unsigned int earthNight = loadTexture("../resources/textures/2k_earth_nightmap.jpg");
	unsigned int earthSpec = loadTexture("../resources/textures/2k_earth_specular_map.png");

	//additional env textures
	unsigned int galaxy = loadTexture("../resources/textures/milkyway_2020_4k.jpg");

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
    setInt(skyboxShader, "earthDay", 1);
    setInt(skyboxShader, "earthNight", 2);
    setInt(skyboxShader, "earthSpec", 3);
    setInt(skyboxShader, "galaxy", 4);
	setVec3(skyboxShader, "lightDir", glm::normalize(lightPos));
	setVec3(skyboxShader, "up", glm::normalize(mainview.up));

	glm::mat4 model = glm::mat4(1.0f);
	// glEnable(GL_PROGRAM_POINT_SIZE);

	state currentState = {
		{7000.0, 0.0, 0.0},
		{0.0, 0.0, -8.0}
	};
	model = glm::translate(model, currentState.pos);

	float time = glfwGetTime();

    initscr();
	unsigned int timewarp = 1;
	std::string input;
	printw("current timewarp ");
    printw("%d", timewarp);
	move(1, 0);
    nodelay(stdscr, 1);
    keypad(stdscr, 1);

    std::regex getFloat("[+-]?([0-9]*[.])?[0-9]+");
    // std::regex getInt("^[1-9]([0-9]*)$");
    std::regex getInt(R"(\d+)");
    bool shipCamera = true;
    float planetAngle = 0;

	while(!glfwWindowShouldClose(window)){
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainview = rotateCamera(window, mainview, deltaTime);	
        mouseCameraOld oldpos = {mainview, float(x), float(y)};
        oldpos = mouseCamera(window, x, y, mainview);
        x = oldpos.x;
        y = oldpos.y;
       	mainview = oldpos.view; 

		int dimensions [4];
		glGetIntegerv(GL_VIEWPORT, dimensions);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), float(dimensions[2]) / float(dimensions[3]), 0.1f, 100.0f);

		//handle inputs
        int key = getch();
        if(key == '\n'){
        	if(input.find("tw") != std::string::npos){
        		std::smatch out;
        		std::regex_search(input, out, getInt);
		        timewarp = std::atoi(out.str().c_str());
		        clrtoeol();
				move(0, 0);
		        clrtoeol();
		        printw("current timewarp ");
		        printw("%d", timewarp);
	    	}else if(input.find("pl") != std::string::npos){
	    		shipCamera = false;
	    		mainview.target = {0, 0, 0};
	    		mainview.distance = 20000;
	    	}else if(input.find("sh") != std::string::npos){
	    		shipCamera = true;
	    		mainview.distance = 3;
	    	}

	        input.clear();
	        move(1, 0);
	        clrtoeol();
        }else if(key == KEY_BACKSPACE){
        	input.pop_back();
        	delch();	
        }else if(key != ERR){
        	// move(1, 0);
        	input.push_back(key);
        }

        refresh();

		glm::vec3 a = gravForce(currentState) / mass + drag(currentState) / mass;
		float newTime = glfwGetTime();
		state oldState = currentState;
		planetAngle -= (newTime - time) * timewarp / 86400;
		currentState = movedt(currentState, a, timewarp * (newTime - time));
		model = glm::translate(model, currentState.pos - oldState.pos);

		if(shipCamera)
			mainview.target = currentState.pos;
		

		time = newTime;
       	glm::vec3 cameraPos = mainview.target + mainview.distance * cameraDir(mainview);

		glm::mat4 view;
		view = glm::lookAt(cameraPos, mainview.target, mainview.up);

		glUseProgram(shaderProgram);

		setMat4(shaderProgram, "model", model);
		setMat4(shaderProgram, "view", view);
		setMat4(shaderProgram, "projection", projection);

		setVec3(shaderProgram, "viewPos", cameraDir(mainview));

		setFloat(shaderProgram, "material.shininess", 32.0f);
		// std::cout << "drawing" << std::endl;
		setMat3(shaderProgram, "normalmtr", glm::inverse(glm::mat3(model)), true);

		draw(shaderProgram, buran, normalMap);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
       	glUseProgram(skyboxShader); 

        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        setMat4(skyboxShader, "view", view);
        setMat4(skyboxShader, "projection", projection);
		setVec3(skyboxShader, "viewDir", glm::normalize(cameraDir(mainview)));
		setVec3(skyboxShader, "cameraPos", cameraPos);
		setFloat(skyboxShader, "angle", planetAngle);

        // skybox cube
        glBindVertexArray(skyboxVAO);

        glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glBindTexture(GL_TEXTURE_2D, starsTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, earthDay);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, earthNight);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, earthSpec);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, galaxy);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // // draw planet
        // glUseProgram(planetShader);

        // glBindVertexArray(planetVAO);
        // // glUseProgram(planetShader);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    // glDeleteProgram(lightShader);

	glfwTerminate();
    endwin();
	return 0;
}