#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class hexPiramid
{
public:
	// static are shared variables acrros this class type
	static bool generatedVBOandVAO;
	static unsigned int vbo;
	static unsigned int vao;

	Shader* shader;
	//XYZ position of Piramid
	glm::vec3 pos;
	hexPiramid();
	void draw();
};
