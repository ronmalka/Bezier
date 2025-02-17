#pragma once
#pragma once
#include "Mesh.h"
#include "shape.h"


class Bezier1D : public Shape
{
private:
     std::vector<glm::mat4> segments;
	 int resT;
	 glm::mat4 M;	 
public:
	enum convexShape { CLINE, CTRIANGLE, CSQUARE };
	Bezier1D(int segNum ,int mode,std::vector<glm::vec3> controlPoints , int viewport);
	IndexedModel GetLine() ;	//generates a model for MeshConstructor Constructor
	void GetControlPoint(int segment, int indx, std::vector<glm::vec3>& lineVertex) ; //returns a control point in the requested segment. indx will be 0,1,2,3, for p0,p1,p2,p3
	glm::vec3 GetPointOnCurve(int indx, float t); //returns point on curve in the requested segment for the value of t
	void SplitSegment(std::vector<glm::vec3> segment, float t, int start);  // split a segment into two parts
	void AddSegment(int seg);
	bool sameLine(std::vector<glm::vec3> segment);
	// add a segment at the end of the curve
	void RemoveSegment();
	void CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1 = false);  //change the line in by using ChangeLine function of MeshConstructor and MoveControlPoint 
	inline float getControlY(int index) { return controlPoints[index].y; }
	inline float getControlX(int index) { return controlPoints[index].x; }
	inline float setControlY(int index, float y) { return controlPoints[index].y =y; }
	inline float setControlX(int index, float x) { return controlPoints[index].x =x; }
	inline std::vector<glm::vec3> getControlPoints() { return controlPoints; }
	int HandleConvexHull(float xpos, float ypos, bool btn_left);
	int findSegmentStartPoint(float xpos, float ypos);
	bool isTriangle(std::vector<glm::vec3>& segment);
	bool checkIfInsideShpae(std::vector<glm::vec3> segment, int convexShape, float xpos, float ypos);
	std::vector<glm::vec2> findRectangle(std::vector<glm::vec3> segment);
	inline int GetSegmentsNum() const { return segNum; }

private:
	~Bezier1D(void);
};

