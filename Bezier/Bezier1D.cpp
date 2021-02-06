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
	
	for (int t = 0; t <= 50; t ++)
	{
		p = GetPointOnCurve(indx, (float)t*0.02f);
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

int Bezier1D::findSegmentStartPoint(float xpos, float ypos)
{

	int start = -1;
	
	for (size_t i = 0; i < controlPointsWorld.size() - 3; i += 3)
	{
		if (controlPointsWorld[i].x < xpos && xpos < controlPointsWorld[i + 3].x)
		{
			
			start = i;
			break;
		}
	}
	return start;
}

bool Bezier1D::sameLine(std::vector<glm::vec3> segment)
{
	float prevAngle = glm::degrees(glm::atan(segment[0].y - segment[1].y, segment[0].x - segment[1].y));
	for (size_t i = 1; i < 3; i++)
	{
		float angle = glm::degrees(glm::atan(segment[i].y - segment[i + 1].y, segment[i].x - segment[i + 1].y));

		if (glm::abs(glm::abs(angle) - glm::abs(prevAngle)) > 1.)
		{
			return false;
		}

		prevAngle = angle;
	}

	return true;

}


float area(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return glm::abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

/* A function to check whether point P(x, y) lies inside the triangle formed
   by A(x1, y1), B(x2, y2) and C(x3, y3) */
bool isInside(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y)
{

	/* Calculate area of triangle ABC */
	float A = area(x1, y1, x2, y2, x3, y3);
	/* Calculate area of triangle PBC */
	float A1 = area(x, y, x2, y2, x3, y3);
	/* Calculate area of triangle PAC */
	float A2 = area(x1, y1, x, y, x3, y3);
	/* Calculate area of triangle PAB */
	float A3 = area(x1, y1, x2, y2, x, y);
	/* Check if sum of A1, A2 and A3 is same as A */
	return (glm::abs(A - (A1 + A2 + A3)) < 1);
}

bool Bezier1D::isTriangle(std::vector<glm::vec3>& segment)
{
	glm::vec3 p0 = segment[0];
	glm::vec3 p1 = segment[1];
	glm::vec3 p2 = segment[2];
	glm::vec3 p3 = segment[3];

	if (isInside(p0.x, p0.y, p1.x, p1.y, p3.x, p3.y, p2.x, p2.y))
	{
		segment.clear();
		segment.push_back(p0);
		segment.push_back(p1);
		segment.push_back(p3);
		segment.push_back(p2);
		return true;
	}
	else if (isInside(p0.x, p0.y, p2.x, p2.y, p3.x, p3.y, p1.x, p1.y))
	{
		segment.clear();
		segment.push_back(p0);
		segment.push_back(p2);
		segment.push_back(p3);
		segment.push_back(p1);
		return true;
	}


	return false;

}

std::vector<glm::vec2> Bezier1D::findRectangle(std::vector<glm::vec3> segment)
{
	float angle = glm::atan(segment[1].y - segment[0].y, segment[1].x - segment[0].x);
	angle = glm::radians(90 - glm::degrees(angle));

	std::vector<glm::vec2> rec;

	float unit = 15.;
	float x = glm::cos(angle) * unit;
	float y = glm::sin(angle) * unit;

	glm::vec2 leftTop = glm::vec2(segment[0].x + x, segment[0].y + y);
	rec.push_back(leftTop);

	glm::vec2 rightTop = glm::vec2(segment[3].x + x, segment[3].y + y);
	rec.push_back(rightTop);

	glm::vec2 leftBottom = glm::vec2(segment[0].x - x, segment[0].y - y);
	rec.push_back(leftBottom);

	glm::vec2 rightBottom = glm::vec2(segment[3].x - x, segment[3].y - y);
	rec.push_back(rightBottom);

	return rec;

}

bool Bezier1D::checkIfInsideShpae(std::vector<glm::vec3> segment, int convexShape, float xpos, float ypos)
{

	std::vector<glm::vec2> rec;
	switch (convexShape)
	{
	case CLINE:
		rec = findRectangle(segment);

		return	isInside(rec[0].x, rec[0].y, rec[1].x, rec[1].y, rec[2].x, rec[2].y, xpos, ypos) ||
			isInside(rec[0].x, rec[0].y, rec[2].x, rec[2].y, rec[3].x, rec[3].y, xpos, ypos) ||
			isInside(rec[0].x, rec[0].y, rec[1].x, rec[1].y, rec[3].x, rec[3].y, xpos, ypos) ||
			isInside(rec[1].x, rec[1].y, rec[2].x, rec[2].y, rec[3].x, rec[3].y, xpos, ypos);
		break;
	case CTRIANGLE:
		return isInside(segment[0].x, segment[0].y, segment[1].x, segment[1].y, segment[2].x, segment[2].y, xpos, ypos);
		break;
	case CSQUARE:
		return	isInside(segment[0].x, segment[0].y, segment[1].x, segment[1].y, segment[2].x, segment[2].y, xpos, ypos) ||
			isInside(segment[0].x, segment[0].y, segment[1].x, segment[1].y, segment[3].x, segment[3].y, xpos, ypos) ||
			isInside(segment[0].x, segment[0].y, segment[2].x, segment[2].y, segment[3].x, segment[3].y, xpos, ypos) ||
			isInside(segment[1].x, segment[1].y, segment[2].x, segment[2].y, segment[3].x, segment[3].y, xpos, ypos);
		break;
	default:
		break;
	}
	return false;
}

void Bezier1D::SplitSegment(std::vector<glm::vec3> segment, float t, int start)
{

	glm::vec3 i0;
	glm::vec3 i1;
	glm::vec3 i2;
	glm::vec3 i3;

	glm::vec3 r0;
	glm::vec3 r1;
	glm::vec3 r2;
	glm::vec3 r3;

	i0 = segment[0];
	r3 = segment[3];
	i1 = (segment[0] + segment[1]) * t;
	r2 = (segment[2] + segment[3]) * t;
	i2 = (i1 + (segment[1] + segment[2]) * t) * t;
	r1 = (r2 + (segment[1] + segment[2]) * t) * t;
	i3 = r0 = (i2 + r1) * t;

	std::vector<glm::vec3> tmpControlPoints = controlPoints;

	controlPoints.clear();

	for (int i = 0; i < start; i++)
	{
		controlPoints.push_back(tmpControlPoints[i]);
	}

	controlPoints.push_back(i0);
	controlPoints.push_back(i1);
	controlPoints.push_back(i2);
	controlPoints.push_back(i3);
	controlPoints.push_back(r1);
	controlPoints.push_back(r2);
	controlPoints.push_back(r3);

	for (int i = start + 4; i < tmpControlPoints.size(); i++)
	{
		controlPoints.push_back(tmpControlPoints[i]);
	}
}

bool Bezier1D::HandleConvexHull(float xpos, float ypos)
{
	int startPoint = findSegmentStartPoint(xpos, ypos);

	if (startPoint == -1) return false;

	int convexShape;
	std::vector<glm::vec3> segment;
	std::vector<glm::vec3> segmentWorld;

	for (size_t i = 0; i < 4; i++)
	{
		segment.push_back(controlPoints[startPoint + i]);
		segmentWorld.push_back(controlPointsWorld[startPoint + i]);
	}


	if (sameLine(segmentWorld))
	{
		convexShape = CLINE;
	}
	else if (isTriangle(segmentWorld))
	{
		convexShape = CTRIANGLE;
	}
	else
	{
		convexShape = CSQUARE;
	}

	if (checkIfInsideShpae(segmentWorld, convexShape, xpos, ypos))
	{
		if (segNum == 6) return false;
		SplitSegment(segment, 0.5f, startPoint);
		return true;
	}

	return false;
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