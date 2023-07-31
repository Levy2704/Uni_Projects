#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class recPiramid
{
public:
	// static are shared variables acrros this class type
	static bool generatedVBOandVAO;
	static unsigned int vbo;
	static unsigned int vao;

	Shader* shader;
	//XYZ position of cube
	glm::vec3 pos;
	recPiramid();
	void draw();
};

