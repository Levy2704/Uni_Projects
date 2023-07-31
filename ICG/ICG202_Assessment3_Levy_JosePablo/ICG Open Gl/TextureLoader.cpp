#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"

unsigned int LoadTexture(const char* fileName, int RGBType)
{
	//generate texture spot in gfx card and get its ID
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);
	// bind texture to make it the active on we're working on
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // t = vertical axis

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// filtering options 
	// TODO setup texture
	// these varis get set when we load the image in (thanks to pointers)
	int width, height, numberofChannels;
	//store image as a big old character string 
	unsigned char* imageData = stbi_load(fileName, &width, &height, &numberofChannels, 0);
	if (imageData)
	{
		// succcess loading image
		// put image into graphics card to store as a texture 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, RGBType, GL_UNSIGNED_BYTE, imageData);

		//generate mipmaps for our texture
		glGenerateMipmap(GL_TEXTURE_2D);

		//Image in gfx card now, so we dont need it locally anymore. Freed that data from RAM
		stbi_image_free(imageData);
	}
	else
	{
		cout << "Failed to load image" << endl;
	}
	return textureID;
}
