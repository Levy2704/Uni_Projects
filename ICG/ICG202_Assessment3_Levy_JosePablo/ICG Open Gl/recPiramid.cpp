#include "recPiramid.h"

bool recPiramid::generatedVBOandVAO = false;
unsigned int recPiramid::vbo = 0;
unsigned int recPiramid::vao = 0;


recPiramid::recPiramid()
{
	if (!recPiramid::generatedVBOandVAO)
	{
		recPiramid::generatedVBOandVAO = true;
		// cube vertex data 
		float textureRecPiramidVertices[] =
		{//        X     Y    Z      R      G       B    Ts     Tt
			// Walls
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			   -0.5f, -0.5f,-0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f,
				0.0f, 0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,

				-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, -0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				 0.0f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

				-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				 0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

				0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			//Base
				-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,1.0f, 0.0f, 0.0f,0.0f, 1.0f
		};
		//VAO AND VBO TIME
		glGenBuffers(1, &recPiramid::vbo);
		glGenVertexArrays(1, &recPiramid::vao);

		glBindVertexArray(recPiramid::vao);
		glBindBuffer(GL_ARRAY_BUFFER, recPiramid::vbo);

		//give data to vbo in gfx card
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureRecPiramidVertices), textureRecPiramidVertices, GL_STATIC_DRAW);


		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recPiramid::ebo);
		//location = 0 X Y Z
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//Location= 1 RGB
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//location = 2 Texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//unbind
		//glBindBuffer(GL_ARRAY_BUFFER, recPiramid::ebo);
		glBindVertexArray(0);
	} // enc generate vbo vao if statement 
}

void recPiramid::draw()
{
	shader->use();
	glad_glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 18); // 36 vertices
}
