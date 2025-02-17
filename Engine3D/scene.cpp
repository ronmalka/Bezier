#define GLEW_STATIC
#include "GL/glew.h"
#include "scene.h"
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"


static void printMat(const glm::mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}

Scene::Scene()
{
	glLineWidth(5);

	pickedShape = -1;
	depth = 0;

	isActive = true;
}

void Scene::AddShapeFromFile(const std::string& fileName, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(fileName, mode));
}

void Scene::AddShape(int type, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(type, mode));
}

void Scene::AddShape(int segNum, int parent, unsigned int mode, std::vector<glm::vec3> controlPoints)
{
	chainParents.push_back(parent);
	bezier1D = new Bezier1D(segNum, mode, controlPoints, 1);
	shapes.push_back(bezier1D);
}

void Scene::AddShapeCopy(int indx, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(*shapes[indx], mode));
}

int Scene::AddShader(const std::string& fileName)
{
	shaders.push_back(new Shader(fileName));
	return (shaders.size() - 1);
}

int  Scene::AddTexture(const std::string& textureFileName, int dim)
{
	textures.push_back(new Texture(textureFileName, dim));
	return(textures.size() - 1);
}

int Scene::AddTexture(int width, int height, unsigned char* data, int mode)
{
	textures.push_back(new Texture(width, height));

	switch (mode)
	{
	case COLOR:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
		break;
	case DEPTH:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		break;
	case STENCIL:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, data);
		break;
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return(textures.size() - 1);
}

int Scene::AddMaterial(unsigned int texIndices[], unsigned int slots[], unsigned int size)
{
	materials.push_back(new Material(texIndices, slots, size));
	return (materials.size() - 1);
}

void Scene::Draw(int shaderIndx, const glm::mat4& Projection, glm::mat4& View, int viewportIndx, unsigned int flags)
{
	glm::mat4 Normal = MakeTrans();

	int p = pickedShape;
	float scale = 1.1;

	for (pickedShape = 0; pickedShape < shapes.size(); pickedShape++)
	{
		if (shapes[pickedShape]->Is2Render(viewportIndx))
		{
			glm::mat4 Model =  shapes[pickedShape]->MakeTrans();
			if (chainParents[pickedShape] == -2) {
				Model = glm::inverse(View) * Model;
			}
			else {
				Model = Normal * Model;
			}
			if (shaderIndx > 0 )
			{	
				if (!(picked.find(pickedShape) != picked.end()))
				{
					Update(Projection, View, Model, shapes[pickedShape]->GetShader());
					shapes[pickedShape]->Draw(shaders[shapes[pickedShape]->GetShader()], false);
				}
				else //If the shape is in picked
				{
					//Clean Stencil Buffer
					glClear(GL_STENCIL_BUFFER_BIT);

					glEnable(GL_STENCIL_TEST);
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

					//Add 1 to every shape drawn
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);

					//Draw the shape
					Update(Projection, View, Model, shapes[pickedShape]->GetShader());
					shapes[pickedShape]->Draw(shaders[shapes[pickedShape]->GetShader()], false);

					//Disable Stencil and draw all not equal to 1
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilMask(0x00);
					glDisable(GL_DEPTH_TEST);

					//Draw the slightly bigger green shape (shader4)
					shapes[pickedShape]->MyScale(glm::vec3(scale, scale, scale));
					Model = Normal * shapes[pickedShape]->MakeTrans();
					Update(Projection, View, Model, 4);
					shapes[pickedShape]->Draw(shaders[4], false);
					//Return to the original size
					shapes[pickedShape]->MyScale(glm::vec3(1 / scale, 1 / scale, 1 / scale));

					//Finish Stencil
					glStencilMask(0xFF);
					glStencilFunc(GL_ALWAYS, 0, 0xFF);
					glEnable(GL_DEPTH_TEST);
					glDisable(GL_STENCIL_TEST);
				}
			}
			else
			{ //picking
				Update(Projection, View, Model, 0);
				shapes[pickedShape]->Draw(shaders[0], true);
			}
		}
	}
	pickedShape = p;
}

void Scene::ShapeTransformation(int type, float amt)
{
	if (glm::abs(amt) > 1e-5 && pickedShape > 0 )
	{
		switch (type)
		{
		case xTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(amt, 0, 0), 0);
			break;
		case yTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, amt, 0), 0);
			break;
		case zTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, 0, amt), 0);
			break;
		case xRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
			break;
		case yRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
			break;
		case zRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 0, 1), 0);
			break;
		case xScale:
			shapes[pickedShape]->MyScale(glm::vec3(amt, 0, 0));
			break;
		case yScale:
			shapes[pickedShape]->MyScale(glm::vec3(0, amt, 0));
			break;
		case zScale:
			shapes[pickedShape]->MyScale(glm::vec3(0, 0, amt));
			break;
		case xMyRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
			break;
		case yMyRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
		default:
			break;
		}
	}

}

bool Scene::Picking(unsigned char data[4])
{
		pickedShape = -1;
		if (data[0] > 0)
		{
			pickedShape = data[0]-1; //r 
			return true;
		}
		return false;
}
//return coordinates in global system for a tip of arm position is local system 
void Scene::MouseProccessing(int button, int xrel, int yrel)
{
	//if (pickedShape == -1)
	//{
	if (button == 1)
	{
		pickedShape = 0;
		ShapeTransformation(xTranslate, xrel / 80.0f);
		pickedShape = -1;
		//MyTranslate(glm::vec3(-xrel / 80.0f, 0, 0), 0);
		//MyTranslate(glm::vec3(0, yrel / 80.0f, 0), 0);
		WhenTranslate();
	}
	else
	{
		pickedShape = 0;
		ShapeTransformation(yRotate, xrel / 2.0f);
		pickedShape = -1;
		//MyRotate(-xrel / 2.0f, glm::vec3(0, 1, 0), 0);
		//MyRotate(-yrel / 2.0f, glm::vec3(1, 0, 0), 1);
		WhenRotate();
	}
	//}

}

void Scene::ZeroShapesTrans()
{
	for (unsigned int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->ZeroTrans();
	}
}

void Scene::ReadPixel()
{
	glReadPixels(1, 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

void Scene::AddShapeViewport(int shpIndx,int viewPortIndx)
{
	shapes[shpIndx]->AddViewport(viewPortIndx);
}

void Scene::RemoveShapeViewport(int shpIndx, int viewPortIndx)
{
	shapes[shpIndx]->RemoveViewport(viewPortIndx);
}

void Scene::BindMaterial(Shader* s, unsigned int materialIndx)
{

	for (size_t i = 0; i < materials[materialIndx]->GetNumOfTexs(); i++)
	{
		materials[materialIndx]->Bind(textures, i);
		s->SetUniform1i("sampler" + std::to_string(i + 1), materials[materialIndx]->GetSlot(i));
	}
}

void Scene::AddShape(int segNum, std::vector<glm::vec3> controlPoints, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	Bezier2D* bezier2D = new Bezier2D(segNum, controlPoints, mode, 1);
	shapes.push_back(bezier2D);
}

Scene::~Scene(void)
{
	for (Shape* shp : shapes)
	{
		delete shp;
	}

	for (Shader* sdr : shaders)
	{
		delete sdr;
	}
	for (Texture* tex : textures)
	{
		delete tex;
	}
	for (Material* mat : materials)
	{
		delete mat;
	}

}



