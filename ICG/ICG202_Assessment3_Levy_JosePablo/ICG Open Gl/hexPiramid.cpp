#include "hexPiramid.h"

bool hexPiramid::generatedVBOandVAO = false;
unsigned int hexPiramid::vbo = 0;
unsigned int hexPiramid::vao = 0;

hexPiramid::hexPiramid()
{
	if (!hexPiramid::generatedVBOandVAO)
	{
		hexPiramid::generatedVBOandVAO = true;
		// cube vertex data 
		float textureCubeVertices[] =
		{//        X     Y      Z      Ts     Tt

			//Top triangle walls
				0.0f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
					
				0.0f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
				// rectangle walls
				-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, -0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				 0.0f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

				 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				//Bottom triangle walls
				0.0f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,

				0.0f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
				-0.5f,-0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f,
				0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
			
// Hexagon
			 //Top triangle
			   0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
			  -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			   0.0f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			//Bottom Triangle 
			  -0.5f,  -0.5f, -0.5f , 1.0f, 0.0f, 0.0f,0.0f, 0.0f, 
			   0.5f, -0.5f,  -0.5f, 1.0f, 0.0f, 0.0f ,1.0f, 0.0f,
			   0.0f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f,1.0f, 1.0f,
			//Rectangle
			   -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		//VAO AND VBO TIME
		glGenBuffers(1, &hexPiramid::vbo);
		glGenVertexArrays(1, &hexPiramid::vao);

		glBindVertexArray(hexPiramid::vao);
		glBindBuffer(GL_ARRAY_BUFFER, hexPiramid::vbo);

		//give data to vbo in gfx card
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCubeVertices), textureCubeVertices, GL_STATIC_DRAW);

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	} // enc generate vbo vao if statement 
}

void hexPiramid::draw()
{
	shader->use();
	glad_glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 30); // 30 vertices
}
