#define GLEW_STATIC
#include "texture.h"
#include "GL/glew.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& fileName, const int dim)
{
	int width, height, numComponents;
	std::string dir[] = {
			fileName+ "right.jpg",
			fileName + "left.jpg",
			fileName + "top.jpg",
			fileName + "bottom.jpg",
			fileName + "front.jpg",
			fileName + "back.jpg"
	};
	unsigned char* data;

	if (dim != 3)
	{
		data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);

		if (data == NULL)
			std::cerr << "Unable to load texture: " << fileName << std::endl;
	}
	

    glGenTextures(1, &m_texture);
	texDimention = dim;
	Bind(m_texture);
	switch (dim)
	{
	case 1:
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	case 3:
		loadCubemap(dir);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	
	}
	if (dim != 3)
	{
		stbi_image_free(data);
	}
	
}

Texture::Texture(int width,int height,unsigned char *data)
{
	glGenTextures(1, &m_texture);
	if (height > 0)
	{
		texDimention = 2;
		Bind(m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{ 
		texDimention = 1;
		Bind(m_texture);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	
}

Texture::Texture(int width, int height)
{
	glGenTextures(1, &m_texture);
	if (height > 0)
	{
		texDimention = 2;
		Bind(m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		texDimention = 1;
		Bind(m_texture);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	switch (texDimention)
	{
	case 1:
		glBindTexture(GL_TEXTURE_1D, m_texture);
		break;
	case 3:
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		break;
	default:
		//int tex = 1;
		glBindTexture(GL_TEXTURE_2D, m_texture);
	
	}
}


void Texture::loadCubemap(std::string dir[]) 
{
	int width, height, numComponents;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load( dir[i].c_str(), &width, &height, &numComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << dir[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


