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
	/*glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/
	unsigned int texIDs[3] = { 0 , 1, 2 };
	unsigned int slots[3] = { 0 , 1, 2 };
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
	AddShader("../res/shaders/basicShader3");
	AddShader("../res/shaders/basicShader4");
	AddTexture("../res/textures/box0.bmp", 2);
	AddTexture("../res/textures/", 3);
	AddTexture("../res/textures/grass.bmp", 2);
	
	AddShape(Cube, -1, TRIANGLES); //0
	AddShape(Axis, -1, LINES); // 1

	AddMaterial(texIDs, slots, 1);
	AddMaterial(texIDs + 1, slots + 1, 1);
	AddMaterial(texIDs + 2, slots + 2, 1);

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
		controlPoints.push_back(glm::vec3(xTrans, yTrans, 0.f));
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

	SetShapeShader(maxPoints + 2, 2);
	AddShapeViewport(maxPoints + 2, 1);
	RemoveShapeViewport(maxPoints + 2, 0);

	AddShape(Plane,-1,TRIANGLES);
	SetShapeShader(maxPoints + 3, 2);
	RemoveShapeViewport(maxPoints + 3, 1);
	RemoveShapeViewport(maxPoints + 3, 0);
	AddShapeViewport(maxPoints + 3, 2);
}

void bezier::Update(const glm::mat4& Projection, const glm::mat4& View, const glm::mat4& Model, const int  shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];
	new_angel = true;
	float r = ((pickedShape + 1) & 0x000000FF) >>  0;
	float g = ((pickedShape + 1) & 0x0000FF00) >>  8;
	float b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx == 1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniformMat4f("Projection", Projection);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	if (shaderIndx == 3) {
		s->SetUniform1i("skybox", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	}
	if (shaderIndx == 0) {
		s->SetUniform4f("lightColor", r / 255.f, g / 255.f, b / 255.f, 1.f);
	}
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

void bezier::moveXYZ(glm::vec4 objLoc,int dir) {
	objLoc *= dir;
	ShapeTransformation(xTranslate, objLoc.x);
	ShapeTransformation(yTranslate, objLoc.y);
	ShapeTransformation(zTranslate, objLoc.z);
}

void bezier::setNewOffset(double xpos, double ypos,bool is3D,bool isRotate,float zoomCo) {
	if (pickedShape != -1) {
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		offset_x = (xpos - old_x) *zoomCo;
		offset_y = (old_y - ypos) *zoomCo;
		if(!isRotate){ 
			ShapeTransformation(xTranslate, offset_x* 0.0045f);
			ShapeTransformation(yTranslate, offset_y* 0.0045f);
		}
		else {
			if (!is3D) {
				ShapeTransformation(xRotate, offset_y / 2.2f);
				ShapeTransformation(yRotate, offset_x / 2.2f);
			}
			else {
				glm::vec4 objLoc = shapes[pickedShape]->MakeTrans() * glm::vec4(0.f, 0.f, 0.f, 1.f);
				moveXYZ(objLoc, -1);
				ShapeTransformation(xRotate, offset_y / 2.2f);
				ShapeTransformation(yRotate, offset_x / 2.2f);
				moveXYZ(objLoc, 1);

			}
		}
		
		if(!is3D) bezier1D->CurveUpdate(pickedShape - 2, offset_x * 0.0045f, offset_y * 0.0045f);
		old_x = xpos;
		old_y = ypos;

	}
}

void bezier::movePointWithAngel(float parentX, float parentY, float angle)
{
	int parentID = chainParents[pickedShape];
	std::vector<int> childs = FindChilds(parentID);


	if (childs.size() == 1)
	{
		return;
	}
	int tmpPicked = pickedShape;
	pickedShape = (pickedShape == childs[0]) ? childs[1] : childs[0];

	ZeroTrans();
	ShapeTransformation(xTranslate, -parentX);
	ShapeTransformation(yTranslate, -parentY);
	ShapeTransformation(zRotate, angle);
	ShapeTransformation(xTranslate, parentX);
	ShapeTransformation(yTranslate, parentY);


	glm::vec4 v = glm::vec4(0., 0., 1., 1.);
	glm::vec4 res = (shapes[pickedShape]->getRot() * shapes[pickedShape]->getTrans() * shapes[pickedShape]->getScl()) * v;

	bezier1D->setControlX(pickedShape - 2, 0.);
	bezier1D->setControlY(pickedShape - 2, 0.);

	bezier1D->CurveUpdate(pickedShape - 2, res.x, res.y);

	pickedShape = tmpPicked;
}


void bezier::pickedMove(double xpos, double ypos,float zoomCo) {
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		offset_x = (xpos - old_x) * 0.0045f * zoomCo;
		offset_y = (old_y - ypos) * 0.0045f * zoomCo;
		for (auto& p : picked) {
			pickedShape = p;
			ShapeTransformation(xTranslate, offset_x);
			ShapeTransformation(yTranslate, offset_y);
			pickedShape = -1;
		}
		old_x = xpos;
		old_y = ypos;

}

void bezier::setNewOffsetWithRotate(float x, float y)
{ 
	if (pickedShape == -1) return;


	float parentX = bezier1D->getControlX(chainParents[pickedShape] - 2);
	float parentY = bezier1D->getControlY(chainParents[pickedShape] - 2);
	float childX = bezier1D->getControlX(pickedShape - 2);
	float childY = bezier1D->getControlY(pickedShape - 2);
	float angleLimit = 0.5;
	float angle;


	if (new_angel)
	{
		angle = glm::atan(childY - parentY, parentX - childX);
		angle = (glm::abs(angle) < angleLimit) ? angleLimit : angle;
		bezier1D->setAngel(angle);
	}
	else
	{
		angle = bezier1D->getAngel();
	}

	angle = (old_x < x) || (old_y < y) ? -angle : angle;
	old_x = x;
	old_y = y;

	ZeroTrans();
	ShapeTransformation(xTranslate, -parentX);
	ShapeTransformation(yTranslate, -parentY);

	ShapeTransformation(zRotate, angle);

	ShapeTransformation(xTranslate, parentX);
	ShapeTransformation(yTranslate, parentY);

	glm::vec4 v = glm::vec4(0., 0., 1., 1.);

	glm::vec4 res = (shapes[pickedShape]->getRot() * shapes[pickedShape]->getTrans() * shapes[pickedShape]->getScl()) * v;

	bezier1D->setControlX(pickedShape - 2, 0.);
	bezier1D->setControlY(pickedShape - 2, 0.);

	bezier1D->CurveUpdate(pickedShape - 2, res.x, res.y);

	new_angel = false;

	if (cont)
	{
		movePointWithAngel(parentX, parentY, angle);
	}

	
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

		child->MyTranslate(glm::vec3(x_off, 0, 0), 0);
		child->MyTranslate(glm::vec3(0, y_off, 0), 0);
		bezier1D->CurveUpdate(childs[0] - 2, x_off, y_off);
	}

	else
	{
		float childX_1 = bezier1D->getControlX(childs[0] - 2);
		float childY_1 = bezier1D->getControlY(childs[0] - 2);

		float childX_2 = bezier1D->getControlX(childs[1] - 2);
		float childY_2 = bezier1D->getControlY(childs[1] - 2);

		float angle = glm::atan(parentY - childY_1, parentX - childX_1);
		float d2 = glm::distance(glm::vec2(childX_2, childY_2), glm::vec2(parentX, parentY));
	    float new_x = parentX + (glm::cos(angle) * d2 )  ;
		float new_y = parentY + (glm::sin(angle) * d2)   ;

		Shape* child = shapes[childs[1]];
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
	offset_y = ((parentY + offset_y) < 0) ? 0 : offset_y;
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

void bezier::Draw(int shaderIndx, const glm::mat4& Projection, glm::mat4& View, int viewportIndx, unsigned int flags) {

	float scale = 1.1;
	bool shouldDelete = false;
	int pckedToDel = -1;

	if (pickedShape > 22 && !(picked.find(pickedShape) != picked.end()))
	{
		picked.insert(pickedShape);
		shouldDelete = true;
		pckedToDel = pickedShape;
	}
	
	Scene::Draw(shaderIndx, Projection, View, viewportIndx, flags);
	
	if (shouldDelete)
	{
		std::set<int>::iterator it; it = picked.find(pckedToDel );
		picked.erase(it);
	}



}

void bezier::HandleConvexHull(float xpos, float ypos)
{
	if (bezier1D->HandleConvexHull(xpos, ypos))
	{
		for (size_t i = 0; i < bezier1D->GetControlPoints().size(); i++)
		{
			RemoveShapeViewport(i + 2, 1);
			ZeroShapeTrans(i + 2);
			SetShapeShader(i + 2, 1);

			SetPickedShape(i + 2);
			ShapeTransformation(xTranslate, bezier1D->GetControlPoints()[i].x);
			ShapeTransformation(yTranslate, bezier1D->GetControlPoints()[i].y);

			SetPickedShape(-1);
			AddShapeViewport(i + 2, 1);
		}

		GetBezier1D()->AddSegment(GetBezier1D()->GetSegmentsNum() + 1);
	}



}


bezier::~bezier(void)
{

}
