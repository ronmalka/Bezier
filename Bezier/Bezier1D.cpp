#include "Bezier/Bezier1D.h"
#include <iostream>

Bezier1D::Bezier1D(int segNum, int mode, std::vector<glm::vec3> controlPoints, int viewport) : Shape(segNum,mode,controlPoints, viewport)
{
	this->mesh = new MeshConstructor(GetLine(), false);
}

IndexedModel Bezier1D::GetLine()
{
	IndexedModel model;
	glm::vec3 color = glm::vec3(1,1,0);
	for (size_t i = 0; i < segNum; i++)
	{
		GetControlPoint(i, i * 3, model.positions);
	}

	model.colors.push_back(color);
	for (unsigned int i = 1; i < model.positions.size(); i++) 
	{
		model.colors.push_back(color);
		model.indices.push_back(i-1);
		model.indices.push_back(i);
	}
	
	return model;
}

//returns a control point in the requested segment. indx will be 0,1,2,3, for p0,p1,p2,p3
void Bezier1D::GetControlPoint(int segment, int indx, std::vector<glm::vec3> &lineVertex) 
{
	glm::vec3 p;
	
	for (float t = 0.0; t < 1.0; t += 0.001)
	{
		p = GetPointOnCurve(indx, t);
		lineVertex.push_back(p);
	}

}

glm::vec3 Bezier1D::GetPointOnCurve(int indx, float t)
{

	glm::vec3 p0 = controlPoints[indx];
	glm::vec3 p1 = controlPoints[indx + 1];
	glm::vec3 p2 = controlPoints[indx + 2];
	glm::vec3 p3 = controlPoints[indx + 3];
	glm::vec3 bezier = pow(1 - t, 3) * p0
		+ 3 * t * pow(1 - t, 2) * p1
		+ 3 * pow(t, 2) * (1 - t) * p2
		+ pow(t, 3) * p3;
	return bezier;
}

void Bezier1D::AddSegment(int seg)
{
	segNum = seg;
	MeshConstructor* old = this->mesh;
	this->mesh = new MeshConstructor(GetLine(), false);
	delete(old);
}

void Bezier1D::RemoveSegment() 
{

}

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1)
{
	controlPoints[pointIndx].x += dx;
	controlPoints[pointIndx].y += dy;
	this->mesh->ChangeLine(GetLine());

}

Bezier1D::~Bezier1D() {}