#pragma once
#include "Bezier1D.h"
#include "Mesh.h"
#include "shape.h"

class Bezier2D : public Shape
{

	void AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf ,int subIndx);  // add a new Nurb to the model
	glm::vec4 CalcNurbs(float s, float t, const std::vector<glm::vec4> subSurf); 
	glm::vec3 CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf);
	void make2DCtrlPoints(int curveIndex, float angle, std::vector<std::vector<glm::vec3>>& pts);  // calculates control points cubic Bezier manifold.

public:
	Bezier2D(void);
	Bezier2D(int segNum, std::vector<glm::vec3> controlPoints, int mode, int viewport=0);
	IndexedModel GetSurface();	//generates a model for MeshConstructor Constructor
	glm::vec3 evalBezierCurve(std::vector<glm::vec3>& p, float t);
	float bernstein(int i, int n, float u);
	glm::vec3 evalBezierPatch(std::vector<std::vector<glm::vec3>>& ps, const float& u, const float& v);
	void makeVertices(int curveIndex, int& startIndex,IndexedModel& idxed);
	glm::vec3 calcDeriv(std::vector<glm::vec3>& p,float t);
	glm::vec3 calcDerivU(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	glm::vec3 calcDerivV(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	glm::vec3 crossDeriv(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	Vertex* GetPointOnSurface(int segmentS, int segmentT, int s, int t);  //returns a point on the surface in the requested segment for value of t and s
	glm::vec3 GetNormal(int segmentS, int segmentT, int s, int t); //returns a normal of a point on the surface in the requested segment for value of t and s
	//void MoveControlPoint(int segmentS, int segmentT, int indx, bool preserveC1);
	~Bezier2D(void);
};