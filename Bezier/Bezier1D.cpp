#include "Bezier/Bezier1D.h"

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
	glm::vec3 bezier = glm::vec3(0,0,0);

	bezier.x = powf((1.0f - t), 3.0f) * p0.x + 3.0f * t * powf((1.0f - t), 2.0f) * p1.x + 3.0f * (1.0f - t) * powf(t, 2.0f) * p2.x + powf(t, 3.0f) * p3.x;
	bezier.y = powf((1.0f - t), 3.0f) * p0.y + 3.0f * t * powf((1.0f - t), 2.0f) * p1.y + 3.0f * (1.0f - t) * powf(t, 2.0f) * p2.y + powf(t, 3.0f) * p3.y;

	return bezier;
}

void Bezier1D::AddSegment(int seg)
{
	segNum = seg;
	this->mesh = new MeshConstructor(GetLine(), false);
}

void Bezier1D::RemoveSegment() 
{

}

Bezier1D::~Bezier1D() {}