#include "bezier.h"
#include <iostream>
#include "GL/glew.h"

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

bezier::bezier() : Scene()
{
	counter = 1;
	
}

//bezier::bezier(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void bezier::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	int N = 3;
	int points = (3 * N) + 1;
	float PI = 3.141592654;

	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	AddTexture("../res/textures/", 3);
	AddTexture("../res/textures/box0.bmp", 2);
	//TextureDesine(840, 840);]

	AddShape(Cube, -1, TRIANGLES);
	AddShape(Axis, -1, LINES);


	for (int i = 0; i < (3 * N) + 1; ++i) {
		AddShape(Octahedron, -1, TRIANGLES);
		pickedShape = i + 2;
		float angle = PI * i / points;
		ShapeTransformation(xTranslate, -points / 2 + i);
		ShapeTransformation(yTranslate, i == points - 1 || i == 0 ? 0 : 2 * sin(angle));
		AddShapeViewport(i + 2, 1);
		RemoveShapeViewport(i + 2, 0);
		
		pickedShape = -1;
	}
	

	AddMaterial(texIDs,slots, 1);
	AddMaterial(texIDs+1, slots + 1, 1);
	//AddMaterial(texIDs + 2, slots + 2, 3);

	SetShapeMaterial(0, 0);
	SetShapeMaterial(1, 1);

	//AddShapeCopy(0, -1, TRIANGLES);

	//SetShapeShader(0, 1);
	//SetShapeShader(1, 1);
	SetShapeShader(0, 2);
	AddShapeViewport(0, 0);
	RemoveShapeViewport(0, 1);
	//SetShapeShader(3, 1);

	SetShapeShader(1, 1);
	AddShapeViewport(1, 1);
	RemoveShapeViewport(1, 0);

	pickedShape = 0;
	ShapeTransformation(zTranslate, 10);
	//pickedShape = 1;
	//ShapeTransformation(yTranslate, 4);
	//pickedShape = 1;
	//ShapeTransformation(xScale, 100);
	//ShapeTransformation(yScale, 100);
	//ShapeTransformation(zScale, 100);
	//pickedShape = 3;
	//ShapeTransformation(xTranslate, 7);
	pickedShape = -1;
	//SetShapeMaterial(0, 0);
	

		
		




}

void bezier::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];

	int r = ((pickedShape) & 0x000000FF) >>  0;
	int g = ((pickedShape) & 0x0000FF00) >>  8;
	int b = ((pickedShape) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();


	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal", Model);
	
	s->SetUniform1i("skybox", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));

	s->SetUniform4f("lightColor", r, g, b, 0);
	s->SetUniform1ui("counter", counter);
	s->SetUniform1f("x", x);
	s->SetUniform1f("y", y);
	s->Unbind();


	

	// ... draw rest of the scene
}

void bezier::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y =  1 - ypos / viewport[3]; 
}

void bezier::WhenRotate()
{
	std::cout << "x "<<x<<", y "<<y<<std::endl;
	
}

void bezier::WhenTranslate()
{
}



void bezier::Motion()
{
	//if(isActive)
	//{
	//	pickedShape = 3;
	//	ShapeTransformation(yRotate, 0.07);
	//}
}

unsigned int bezier::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] =  (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

bezier::~bezier(void)
{

}
