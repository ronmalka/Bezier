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
	int N = 6;  // max segments
	int seg = 3;
	int points = (3 * seg) + 1;
	int maxPoints = (3 * N) + 1;
	float PI = 3.141592654;
	std::vector<glm::vec3> controlPoints; 

	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShaderTex");
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	AddTexture("../res/textures/grass.bmp", 2);
	AddTexture("../res/textures/", 3);
	AddShape(Cube, -1, TRIANGLES); //0
	AddShape(Axis, -1, LINES); // 1

	for (size_t i = 0; i < maxPoints; i++) // init max points shape
	{
		AddShape(Octahedron, i%3 == 0 ? -1 : (i%3 == 1 ? (i+1) : (i+3)), TRIANGLES);
		pickedShape = i + 2;
		RemoveShapeViewport(i + 2, 0);
		RemoveShapeViewport(i + 2, 1);
		SetShapeShader(i+2,1);
		pickedShape = -1;
	}
	for (int i = 0; i < points; ++i) { 
		pickedShape = i + 2;
		float angle = PI * i / points;
		float xTrans = (((float)-points / 2.f) + i)/8.8f;
		float yTrans = i == (points - 1) || i == 0 ? 0 : (2.f + 3.f * sin(angle))/8.8f;
		ShapeTransformation(xTranslate, xTrans);
		ShapeTransformation(yTranslate, yTrans);
		SetShapeMaterial(i+2,0);
		controlPoints.push_back(glm::vec3(xTrans, yTrans, pickedShape));
		AddShapeViewport(i + 2, 1);
		pickedShape = -1;
	}

	AddMaterial(texIDs,slots, 1);
	AddMaterial(texIDs+1, slots + 1, 1);
	AddShape(seg, -1, LINE_STRIP, controlPoints);

	SetShapeMaterial(0, 1);
	SetShapeShader(0, 3);
	AddShapeViewport(0, 0);
	RemoveShapeViewport(0, 1);

	SetShapeShader(1, 2);
	AddShapeViewport(1, 1);
	RemoveShapeViewport(1, 0);

	SetShapeShader(maxPoints + 2, 1);
	AddShapeViewport(maxPoints + 2, 1);
	RemoveShapeViewport(maxPoints + 2, 0);

}

void bezier::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];

	float r = ((pickedShape + 1) & 0x000000FF) >>  0;
	float g = ((pickedShape + 1) & 0x0000FF00) >>  8;
	float b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if (shaderIndx != 2)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal", Model);
	s->SetUniform1i("skybox", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	s->SetUniform4f("lightColor", r/255.f, g/255.f, b/ 255.f, 1.f);
	s->SetUniform1ui("counter", counter);
	s->SetUniform1f("x", x);
	s->SetUniform1f("y", y);
	s->Unbind();

	// ... draw rest of the scene
}

void bezier::updatePressedPos(double xpos, double ypos) {
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	old_x = xpos;
	old_y = ypos;
}

void bezier::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y =  1 - ypos / viewport[3]; 
}

void bezier::setNewOffset(double xpos, double ypos) {
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	offset_x = (xpos - old_x)  * 0.0045f;
	offset_y = (old_y - ypos) * 0.0045f;
	old_x = xpos;
	old_y = ypos;
	ShapeTransformation(xTranslate, offset_x);
	ShapeTransformation(yTranslate, offset_y);
	bezier1D->CurveUpdate(pickedShape - 2, offset_x, offset_y);
}

void bezier::setNewOffsetWithRotate(double x, double y) 
{ 

	float parentX = bezier1D->getControlX(chainParents[pickedShape]-2);
	float parentY = bezier1D->getControlY(chainParents[pickedShape]-2);
	float childX = bezier1D->getControlX(pickedShape -2);
	float childY = bezier1D->getControlY(pickedShape-2);
	float radius = glm::distance(glm::vec2(parentX, parentY), glm::vec2(childX, childY));
	float angle = glm::atan( parentY - childY, parentX - childX);
	angle = (x < old_x) ? -angle : angle;
	float new_x =  glm::cos(angle) * radius * 0.0045f;
	float new_y =  glm::sin(angle) * radius * 0.0045f;
	//radius = glm::distance(glm::vec2(parentX, parentY), glm::vec2(new_x, new_y));
	//std::cout << "new_x: " << new_x << " new_y: " << new_y << " angle: " << angle << std::endl;
	//std::cout << "radius: " << radius << std::endl;
	ShapeTransformation(xTranslate, -parentX);
	ShapeTransformation(yTranslate, -parentY);

	ShapeTransformation(zRotate, angle);
	//ShapeTransformation(yRotate, angle);

	ShapeTransformation(xTranslate, parentX);
	ShapeTransformation(yTranslate, parentY);

	bezier1D->CurveUpdate(pickedShape - 2, new_x, new_y);

	old_x = x;
	old_y = y;
}

std::vector<int> bezier::FindChilds(int parent)
{
	std::vector<int> res;
	int points = (3 * bezier1D->GetSegmentsNum()) + 3;

	for (size_t i = 0; i < points; i++)
	{
		//std::cout << "id: " << i <<" his child: " << chainParents[i] << std::endl;
		
		if (chainParents[i] == parent)
		{
			//std::cout << "child: " << i <<  std::endl;
			res.push_back(i);
		}
	}

	return res;
}

void bezier::AlignPoints()
{
	std::vector<int> childs = FindChilds(pickedShape);

	float parentX = bezier1D->getControlX(pickedShape - 2);
	float parentY = bezier1D->getControlY(pickedShape - 2);

	if (childs.size() == 1)
	{
		float childX = bezier1D->getControlX(childs[0] - 2);
		float childY = bezier1D->getControlY(childs[0] - 2);
		float d = glm::distance(glm::vec2(parentX, parentY), glm::vec2(childX, childY)) ;
		float x_off = parentX - childX;
		float y_off = d - childY;
		Shape* child = shapes[childs[0]];

		child->MyTranslate(glm::vec3(x_off, 0, 0), 1);
		child->MyTranslate(glm::vec3(0, y_off, 0), 1);

		/*bezier1D->setControlX(childs[0] - 2, parentX);
		bezier1D->setControlY(childs[0] - 2, d);*/



		bezier1D->CurveUpdate(childs[0] - 2, x_off, y_off);
	}

	else
	{
		float childX_1 = bezier1D->getControlX(childs[0] - 2);
		float childY_1 = bezier1D->getControlY(childs[0] - 2);

		float childX_2 = bezier1D->getControlX(childs[1] - 2);
		float childY_2 = bezier1D->getControlY(childs[1] - 2);

		float angle = glm::atan(parentY - childY_1, childX_1 - parentX );
		float d2 = glm::distance(glm::vec2(childX_2, childY_2), glm::vec2(parentX, parentY));

		float new_x = (glm::cos(angle) * d2)  ;
		float new_y = parentY + (glm::sin(angle) * d2) ;
		
		if (parentX > childX_1)
		{
			new_x += parentX;
		}
		else
		{
			new_x = parentX - new_x;
		}

		Shape* child = shapes[childs[1]];
		float nd2 = glm::distance(glm::vec2(parentX, parentY), glm::vec2(-new_x - childX_2, -new_y - childY_2));
		std::cout << "child1: " << childs[0] << " child2: " << childs[1] << std::endl;
		std::cout << "parentX: " << parentX << " parentY: " << parentY << std::endl;
		std::cout << "childX_1: " << childX_1 << " childY_1: " << childY_1 << std::endl;
		std::cout << "childX_2: " << childX_2 << " childY_2: " << childY_2 << std::endl;
		std::cout << "nd2: " << nd2 << " d2: " << d2 << std::endl;
		std::cout << "new_x: " << new_x << " new_y: " << new_y << std::endl;
		std::cout << "cos: " << glm::cos(angle) << " sin: " << glm::sin(angle) << std::endl;
		std::cout << "angle: " << angle <<  std::endl;

		child->ZeroTrans();
		child->MyTranslate(glm::vec3(new_x, new_y, 0), 0);
	

		bezier1D->setControlX(childs[1] - 2, 0. );
		bezier1D->setControlY(childs[1] - 2, 0. );

		bezier1D->CurveUpdate(childs[1] - 2, new_x, new_y);

	}
	
}

void bezier::setNewOffsetWithChilds(double x, double y)
{
	std::vector<int> childs = FindChilds(pickedShape);

	float parentX = bezier1D->getControlX(pickedShape - 2);
	float parentY = bezier1D->getControlY(pickedShape - 2);

	float childX_1 = bezier1D->getControlX(childs[0] - 2);
	float childY_1 = bezier1D->getControlY(childs[0] - 2);

	Shape* child1 = shapes[childs[0]];


	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	offset_x = (x - old_x) * 0.0045f;
	offset_y = (old_y - y) * 0.0045f;
	old_x = x;
	old_y = y;
	ShapeTransformation(xTranslate, offset_x);
	ShapeTransformation(yTranslate, offset_y);

	child1->MyTranslate(glm::vec3(offset_x, offset_y, 0), 0);
	bezier1D->setControlX(childs[0] - 2, offset_x + childX_1);
	bezier1D->setControlY(childs[0] - 2, offset_y + childY_1);

	if (childs.size() > 1)
	{
		float childX_2 = bezier1D->getControlX(childs[1] - 2);
		float childY_2 = bezier1D->getControlY(childs[1] - 2);
		Shape* child2 = shapes[childs[1]];

		child2->MyTranslate(glm::vec3(offset_x, offset_y, 0), 0);
		bezier1D->setControlX(childs[1] - 2, offset_x + childX_2);
		bezier1D->setControlY(childs[1] - 2, offset_y + childY_2);

	}

	bezier1D->CurveUpdate(pickedShape - 2, offset_x, offset_y);
}

void bezier::WhenRotate()
{
}

void bezier::WhenTranslate()
{

}

void bezier::WhenPicked()
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
