#pragma once
#include "Bezier1D.h"
#include "Mesh.h"
#include "shape.h"

class Bezier2D : public Shape
{
	void make2DCtrlPoints(int curveIndex, float angle, std::vector<std::vector<glm::vec3>>& pts);  // calculates control points cubic Bezier manifold.

public:
	Bezier2D(int segNum, std::vector<glm::vec3> controlPoints, int mode, int viewport = 0);
	IndexedModel GetSurface();	//generates a model for MeshConstructor Constructor
	glm::vec3 evalBezierCurve(std::vector<glm::vec3>& p, float t);
	glm::vec3 evalBezierPatch(std::vector<std::vector<glm::vec3>>& ps, const float& u, const float& v);
	void makeVertices(int curveIndex, int& startIndex, IndexedModel& idxed);
	glm::vec3 calcDeriv(std::vector<glm::vec3>& p, float t);
	glm::vec3 calcDerivU(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	glm::vec3 calcDerivV(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	glm::vec3 crossDeriv(std::vector<std::vector<glm::vec3>>& grid, float u, float v);
	~Bezier2D(void);
};