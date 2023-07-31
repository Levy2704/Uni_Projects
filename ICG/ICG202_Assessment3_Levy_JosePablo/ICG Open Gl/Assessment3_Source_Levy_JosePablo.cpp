#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include "Shader.h"
#include <stdlib.h> //srand, rand
#include <time.h> //get seconds since start of the year
#include <list>
#include "TextureLoader.h"

#include <glm/glm.hpp>//basic glm maths functions
#include <glm/gtc/matrix_transform.hpp>//matrix functions
#include <glm/gtc/type_ptr.hpp>//convert glm types to opengl types

#include "recPiramid.h"
#include "hexPiramid.h"

using namespace std;

//Vertex Shader Program Source Code
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"//try pass xyz values for point into this location
"\n"
"void main()\n"
"{\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"//vertex shader must set vec4 gl_position for shape assembly
"}\n\0";

//Fragment Shader Source Code
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"//fragment shader must out a vec4 for rendering a pixel
"\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"//output 1 colour per pixel drawn
"}\n\0";

//which shape to draw
int currentShape = 1;

//user input function
void processInputs(GLFWwindow* window);

void main()
{
	//set seed value for random number set. If use same number, will get same number sets
	srand(time(NULL));

	glfwInit();
	//tell glfw which version of openGL to use
	//we are using opengl 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //first 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//the .3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core

	//build a window
	GLFWwindow* window 
		= glfwCreateWindow(1280, 720, " Assessment2 ICG", NULL, NULL);
	//did building a window fail?!
	if (window == NULL)
	{
		cout << "Window failed to build!!" << endl;
		glfwTerminate();
		system("pause");
		return; //exit program early
	}
	
	//make this the main window
	glfwMakeContextCurrent(window);

	//glad setup all the opengl functions for me :D
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		//if it failed, exit early
		cout << "Glad failed to load opengl Functions" << endl;
		glfwTerminate();
		system("pause");
		return;
	}
	//tell opengl how much of the window to use
	glViewport(0, 0, 1280, 720);//x,y,w,h

	//SHADERS
	//generate shader ID for vertex shader
	int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);//gives id of new shader
	//give shader source code to gfx card
	//params: which shader id, how many strings, mem addr of source code, how long is string
	//										note: if last is NULL source must end in \0
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	//compile it!
	glCompileShader(vertexShaderID);

	//check for errors
	int success;
	char infoLog[512];
	//check compile status
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	//if failed
	if (!success) {
		//get error message and display it
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		cout << "Vertex shader error: " << infoLog << endl;
		system("pause");
	}

	//fragment shader
	int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);
	//check compile status
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	//if failed
	if (!success) {
		//get error message and display it
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		cout << "Fragment shader error: " << infoLog << endl;
		system("pause");
	}
	//SHADER PROGRAM
	int shaderProgramID = glCreateProgram();
	//attach our shaders to it
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	//link all shaders together
	glLinkProgram(shaderProgramID);
	//check for linking errors
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		cout << "Shader program linking error: " << infoLog << endl;
		system("pause");
	}
	//once linked together, dont need smaller parts anymore
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//GET OTHER SHADERS
	Shader shaderProgram1("vertexShader1.txt", "fragmentShader1.txt");
	Shader shaderProgram2("vertexShader1.txt", "fragmentShader2.txt");
	Shader shaderProgram3("vertexShader2.txt", "fragmentShader1.txt");
	Shader shaderProgram4("textureVertexShader1.txt", "textureFragmentShader1.txt");
	Shader shaderProgram5("textureVertexShader2.txt", "textureFragmentShader2.txt");
	Shader projectionShader1("projectionVertexShader.txt", "textureFragmentShader1.txt");
	Shader projectionShader2("projectionVertexShader.txt", "textureFragmentShader2.txt");

	//TRIANGLE VERTICES
	//NOTE: values fitting in Normalised Device Coordinate values
	float vertices[] = {
		//X    Y    Z
		0.5, -0.5, 0, // bottom right
		-0.5, -0.5, 0, //bottom left
		0.0, 0.5, 0 //top
	};//<===semicolon

	//VBO - Vertex Buffer Object
	//we need to store vertice data thats in RAM in our VRAM so we can render with it
	unsigned int vboID1;
	//generate a VBO and it'll set vboID1's value
	//params: how many vbos to make, mem address of where to store ids locally
	glGenBuffers(1, &vboID1);
	//hypothetical: generate 6 vbos at once
	//unsigned int vbos[6];
	//gelGenBuffers(6, vbos);

	//VAO - Vertex Array Object, explain how buffer data feeds into vertex shader
	unsigned int vaoID1;
	glGenVertexArrays(1, &vaoID1);
	//bind to make it the current VAO
	glBindVertexArray(vaoID1);

	//only 1 current VBO active at a time for operations, bind it to make it active
	glBindBuffer(GL_ARRAY_BUFFER, vboID1);
	//pass data to the currently binded VBO
	//params: what type of type of buffer
	// size of data to buffer
	//the data
	//data type: static, dynamic, stream
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//tell vao how to feed XYZ values into vertex shader
	//params: location slot to pass these values into (location = 0 in vertex shader)
	//		aPos is a vec3, so we want 3 bits of data
	//		data type? (vec3 is 3 floats)
	//		normalise the data? (cut off values to -1 or 1 as min and max)
	//		stride, how many jumps from this first xyz to the next in the array
	//		how many jumps into the array is the first xyz. NOTE: expecting a void* type :/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//turn on location  = 0 in vao
	glEnableVertexAttribArray(0);

	//unbind VAO for safety so I dont mess it up
	glBindVertexArray(0);

	//Rectangle Vertices
	float rectVertices[] =
	{
		// x  y   z
		0.5, 0.5, 0, //top right        index: 0
		0.5, -0.5, 0, //bottom right    index: 1
		-0.5, -0.5, 0, //bottom left    index: 2
		-0.5, 0.5, 0 //top left         index: 3
	};

	//order of indexes to help build triangles
	unsigned int indices[] = {
		0, 1, 3, //first triangle
		1,2,3 //second triangle
	};

	//VAO and VBO and EBO
	unsigned int vboID2;
	glGenBuffers(1, &vboID2);

	unsigned int vaoID2;
	glGenVertexArrays(1, &vaoID2);
	//ebo-elemental buffer object stores index data for vertex construction
	unsigned int eboID1;
	glGenBuffers(1, &eboID1);

	//bind em
	glBindVertexArray(vaoID2);
	glBindBuffer(GL_ARRAY_BUFFER, vboID2);
	//buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
	//bind and buffer ebo data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//xyz into vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind vao
	glBindVertexArray(0);

	//RAINBOW TRIANGLE
	//vertexes for both coordinates and colours
	float rainbowTriangleVertices[] = {
		//x  y   z        r  g  b
		0.5f, -0.5f, 0,   1, 0, 0,
		-0.5f, -0.5f, 0,  0, 1, 0,
		0.0f, 0.5f, 0,     0, 0, 1
	};//<========= semicolon

	unsigned int rainbowVBO;
	glGenBuffers(1, &rainbowVBO);
	unsigned int rainbowVAO;
	glGenVertexArrays(1, &rainbowVAO);

	glBindVertexArray(rainbowVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rainbowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rainbowTriangleVertices), rainbowTriangleVertices, GL_STATIC_DRAW);
	 
	//get XYZ values into location 0 on vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//RGB to location 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//get stb to load images the right way up for opengl
	stbi_set_flip_vertically_on_load(true);

	//LOAD TEXTURES
	unsigned int texture1 = LoadTexture("crate.jpg", GL_RGB);
	unsigned int texture2 = LoadTexture("car.jpg", GL_RGB);
	unsigned int texture3 = LoadTexture("concrete.jpg", GL_RGB);
	
	

	// Color rectangle with textures---------------------
	//vertices for textured rectangle
	float colourRectVertices[] = {
		// positions // colors // texture coords
		//x    y    z      R     G    B      Ts   Tt
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right           0
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right       1
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left       2
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left            3
	};

	//VBO, EBO, VAO
	unsigned int textureRectVBO, textureRectVAO, textureRectEBO;
	glGenBuffers(1, &textureRectVBO);
	//glGenBuffers(1, &textureRectEBO);
	glGenVertexArrays(1, &textureRectVAO);

	glBindVertexArray(textureRectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textureRectVBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(colourRectVertices), colourRectVertices, GL_STATIC_DRAW);

	//Turns out I did need to rebind the EBO from before to associate it with the current VAO too
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID1);
	//XYZ values into location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//RGB values into location = 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	//ST(texture coordinates) values into location = 2
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Piramid Time------------------------------------------------
	list <recPiramid> rec;
	//Position for 10 cubes
	glm::vec3 recPiramidPositions[] =
	{
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	//Build 10 rectangular Piramids
	for (int i = 0; i < 10; i++)
	{
		recPiramid recs;
		recs.shader = &projectionShader1;
		recs.pos = recPiramidPositions[i];

		//add cubes to the list
		rec.push_back(recs);
	}

	// Hexagon Piramid---------------------------------------
	list <hexPiramid> hex;
	//Position for 10 hex

	glm::vec3 hexPositions[] =
	{
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -4.5f),
	glm::vec3(3.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	//Build 10 cubes
	for (int i = 0; i < 10; i++)
	{
		hexPiramid hexa;
		hexa.shader = &projectionShader2;
		hexa.pos = hexPositions[i];

		//add cubes to the list
		hex.push_back(hexa);
	}
	//turn on 2 buffering(sort veries based on 2 axis)
	glEnable(GL_DEPTH_TEST);

	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		//inputs first!
		processInputs(window);

		//RENDER STUFF
		//set clear colour
		glClearColor(0, 0, 1, 1);//rgba values 0-1 e.g 0.5
		//clear the screen with clear colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //BITWISEMASK

		if (currentShape == 1) {
			//for echa pyramid
			for (hexPiramid hexa : hex)
			{

				glm::mat4 model = glm::mat4(1.0);
				// move cube to where it should go
				model = glm::translate(model, hexa.pos);
				//skip scaling
				//Roate on Y axis
				model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
				// setup camera view matrix
				glm::mat4 view = glm::mat4(1.0);
				//move camera in reverse to what you think you should
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
				// setup projection for perspective and NDC conversion
				//                                           FOV             aspect ration near & far place
				glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
				//need to pass in values for uniforms
				glUniformMatrix4fv(glGetUniformLocation(hexa.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(glGetUniformLocation(hexa.shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(hexa.shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

				//TEXTURES
				glActiveTexture(GL_TEXTURE0);
				//bind our textureID to that slot
				glBindTexture(GL_TEXTURE_2D, texture3);
				int texture1UniformLocation = glGetUniformLocation(hexa.shader->ID, "texture1");
				glUniform1i(texture1UniformLocation, 0);//<==== set to slot zero

				//set active shader slot to slot 1
				glActiveTexture(GL_TEXTURE1);
				//bind our textureID to that slot
				glBindTexture(GL_TEXTURE_2D, texture2);
				//tell fragment shader that we want its uniform texture1 var to be
				//referencing slot 1
				int texture2UniformLocation = glGetUniformLocation(hexa.shader->ID, "texture2");
				glUniform1i(texture2UniformLocation, 1);//<==== set to slot one

				//DRAW Pyramid
				hexa.draw();
			}
		}
		if (currentShape == 2)
		{
			//for echa pyramid
			for (recPiramid recs : rec)
			{
				glm::mat4 model = glm::mat4(1.0);
				// move cube to where it should go
				model = glm::translate(model, recs.pos);
				//skip scaling
				//Roate on Y axis
				model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
				// setup camera view matrix
				glm::mat4 view = glm::mat4(1.0);
				//move camera in reverse to what you think you should
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
				// setup projection for perspective and NDC conversion
				//                                           FOV             aspect ration near & far place
				glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
				//need to pass in values for uniforms
				glUniformMatrix4fv(glGetUniformLocation(recs.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(glGetUniformLocation(recs.shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(recs.shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

				//TEXTURES
				glActiveTexture(GL_TEXTURE0);
				//bind our textureID to that slot
				glBindTexture(GL_TEXTURE_2D, texture1);
				int texture1UniformLocation = glGetUniformLocation(recs.shader->ID, "texture1");
				glUniform1i(texture1UniformLocation, 0);//<==== set to slot zero

				//set active shader slot to slot 1
				glActiveTexture(GL_TEXTURE1);
				//bind our textureID to that slot
				glBindTexture(GL_TEXTURE_2D, texture2);
				//tell fragment shader that we want its uniform texture1 var to be
				//referencing slot 1
				int texture2UniformLocation = glGetUniformLocation(recs.shader->ID, "texture2");
				glUniform1i(texture2UniformLocation, 1);//<==== set to slot one

				//DRAW Pyramid
				recs.draw();
			}
		}
		if (currentShape == 4) {
			//DRAW RECTANGLE
			//glUseProgram(shaderProgramID);
			//shaderProgram1.use();
			shaderProgram2.use();
			//work out green value
			//RAND - gives number between 0 and max int value
			//%101 gives values between 0-100
			// then divide by 100. making sure its a float so the decimals dont get cut
			float redValue = (rand() % 101) / 100.0f;
			float greenValue = (rand() % 101) / 100.0f; ;// sin(glfwGetTime()) / 2.0f + 0.5f;
			float blueValue = (rand() % 101) / 100.0f;

			//ask shader for variable location of our uniform called ourColour
			int ourColourUniformLocation = glGetUniformLocation(shaderProgram2.ID, "ourColour");
			//set uniform on shader
			//										R      G         B     A
			glUniform4f(ourColourUniformLocation, redValue, greenValue, blueValue, 1.0f);

			glBindVertexArray(vaoID2);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		if (currentShape == 5) {
			//RAINBOW TRIANGLE
			shaderProgram3.use();
			glBindVertexArray(rainbowVAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		if (currentShape == 3)
		{
			shaderProgram5.use();

			// Transform Matrix
			glm::mat4 transform = glm::mat4(1.0f); // build identity
			//move rectangle
			transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
			//rotation                          amount                  multiply to each axis we just doing
			transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
			//Scale
			transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));

			int transformUniformLocation = glGetUniformLocation(shaderProgram5.ID, "transform");
			glUniformMatrix4fv(transformUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));

			//make texture slot 0 active(makes it the one we're working on)
			glActiveTexture(GL_TEXTURE0);
			//bind our crate texture to that slot
			glBindTexture(GL_TEXTURE_2D, texture1);
			//tell our fragmentshader to use slot 0 for uniform sampler2D texture1;
			int texture1UniformLocation = glGetUniformLocation(shaderProgram5.ID, "texture1");
			glUniform1i(texture1UniformLocation, 0);//tell it to use texture slot 0

			//make texture slot 1 active
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
			int texture2UniformLocation = glGetUniformLocation(shaderProgram5.ID, "texture2");
			glUniform1i(texture2UniformLocation, 1);
			
			glBindVertexArray(textureRectVAO);
			//draw
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		
		
		//process polled events
		glfwPollEvents();

		//use double buffering, swap the buffers to replace
		//screen with things we've been rendering to the 
		//off buffer
		glfwSwapBuffers(window);
	}
	//cleanup
	glfwTerminate();
	system("pause");
}

void processInputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		currentShape = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		currentShape = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		currentShape = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		currentShape = 4;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		currentShape = 5;
}