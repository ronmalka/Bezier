#define GLEW_STATIC
#include "GL/glew.h"
#include "shape.h"


Shape::Shape(const Shape& shape, unsigned int mode)
{

	mesh = new MeshConstructor(*shape.mesh);
	//tex = shape.tex;
	isCopy = true;
	this->mode = mode;
	viewports = 1;
	materialID = shape.materialID;
	shaderID = shape.shaderID;

}

Shape::Shape(const std::string& fileName, unsigned int mode) {
	mesh = new MeshConstructor(fileName);
	isCopy = false;
	this->mode = mode;
	viewports = 1;
	materialID = 0;
	shaderID = 1;

}

Shape::Shape(const int SimpleShapeType, unsigned int mode)
{
	mesh = new MeshConstructor(SimpleShapeType);
	//mesh->Bind();
	this->mode = mode;
	isCopy = false;
	viewports = 1;
	shaderID = 1;
	materialID = 0;
}
Shape::Shape(int segNum, int mode, std::vector<glm::vec3> controlPoints, int viewport)
{
	this->viewports = viewport;
	this->mode = mode;
	this->controlPoints = controlPoints;
	this->segNum = segNum;
	isCopy = false;
	shaderID = 1;
	materialID = 0;
}

Shape::Shape(int segNum, std::vector<glm::vec3> controlPoints, int mode, int viewport) {
	this->viewports = viewport;
	this->mode = mode;
	this->controlPoints = controlPoints;
	this->segNum = segNum;
	isCopy = false;
	shaderID = 1;
	materialID = 0;
}
void Shape::Draw(const Shader* shader, bool isPicking)
{
	
	shader->Bind();
	mesh->Bind();
	/*if(isCopy)
		glDrawArrays(GL_TRIANGLES, 0, indicesNum);
	else*/
	glDrawElements(mode, mesh->GetIndicesNum(), GL_UNSIGNED_INT, 0);
	mesh->Unbind();
}

Shape::~Shape(void)
{
	if (!isCopy)
	{
		if (mesh)
			delete mesh;
	}
}

