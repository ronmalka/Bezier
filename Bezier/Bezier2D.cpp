#include "Bezier\Bezier2D.h"
#include "glm/gtx/rotate_vector.hpp"
#define CALC_OFFSET(a,i) ((a) >= 1600 * i ? (a - 1580) : (a))
#define CALC_KOFFSET(a,i) (a > (segNum*1600) ? (a - (segNum*1600)-19) : a)
#define CALC_TEX(i) (i == 0 ? glm::vec2(0.f,0.f) : i==1 ? glm::vec2(1.f,0.f) : i==2 ? glm::vec2(1.f,1.f) : glm::vec2(0.f,1.f))

Bezier2D::Bezier2D(int segNum, std::vector<glm::vec3> controlPoints, int mode, int viewport) : Shape(segNum, controlPoints, mode, viewport)
{
	this->mesh = new MeshConstructor(GetSurface(), true);
}

glm::vec3 Bezier2D::calcDeriv(std::vector<glm::vec3>& p, float t)
{ // Calc General Derive Of Curve
	return -3 * pow(1 - t, 2) * p[0]
		+ (3 * pow(1 - t, 2) - 6 * t * (1 - t)) * p[1]
		+ (6 * t * (1 - t) - 3 * pow(t, 2)) * p[2]
		+ 3 * pow(t, 2) * p[3];
}

glm::vec3 Bezier2D::calcDerivU(std::vector<std::vector<glm::vec3>>& grid, float u, float v) {
	//Gets 2D grid, Calculate V (Horizontal) and then Deriving By U
	//Move Horizontal - V 
	std::vector<glm::vec3> vMove;
	for (int i = 0; i < 4; ++i) {
		std::vector<glm::vec3> v_i_Move;
		for (int j = 0; j < 4; ++j) {
			v_i_Move.push_back(grid[j][i]);
		}
		vMove.push_back(evalBezierCurve(v_i_Move, v));
	}

	//Deriving with u
	return calcDeriv(vMove, u);
}

glm::vec3 Bezier2D::calcDerivV(std::vector<std::vector<glm::vec3>>& grid, float u, float v) {
	//Gets 2D grid, calculate curve on U axis and then derive by V (Vertical)
	//Move Horizontal - U - By Moving u on all 4 Curves
	std::vector<glm::vec3> uMove;
	for (int i = 0; i < 4; ++i) {
		uMove.push_back(evalBezierCurve(grid[i], u));
	}

	//Deriving with v
	return calcDeriv(uMove, v);
}

glm::vec3 Bezier2D::crossDeriv(std::vector<std::vector<glm::vec3>>& grid, float u, float v) {
	return -glm::normalize(glm::cross(calcDerivV(grid, u, v), calcDerivU(grid, u, v)));
}

glm::vec3 Bezier2D::evalBezierCurve(std::vector<glm::vec3>& p, float t)
{ // P is 4 Control Points of Bezier Curve
	glm::vec3 bezier = pow(1 - t, 3) * p[0]
		+ 3 * t * pow(1 - t, 2) * p[1]
		+ 3 * pow(t, 2) * (1 - t) * p[2]
		+ pow(t, 3) * p[3];
	return bezier;
}



glm::vec3 Bezier2D::evalBezierPatch(std::vector<std::vector<glm::vec3>>& grid, const float& u, const float& v)
{	//Recives 4 Curves and preform 2d movement - first move point in u axis and then v axis

	////Move Horizontal - U - By Moving u on all 4 Curves
	std::vector<glm::vec3> uMove;
	for (int i = 0; i < 4; ++i) {
		uMove.push_back(evalBezierCurve(grid[i], u));
	}

	//Move Vertical - V - By Moving v on the output of the movement (uMove)
	return evalBezierCurve(uMove, v);
}


void Bezier2D::make2DCtrlPoints(int curveIndex, float startAngle, std::vector<std::vector<glm::vec3>>& grid)
{ //Input: 1D Control Points of Bezier1D (shape->controlPoints[curveIndex*3 + i])
  //Output: 2D Control Points of Bezier2D (1/4 Sphere) in 0deg,30deg,60deg,90deg
	std::vector<glm::vec3> pts;
	for (int i = 0; i < 4; i++) {
		pts.push_back(glm::rotateZ(glm::vec3(controlPoints[curveIndex * 3 + i].x, controlPoints[curveIndex * 3 + i].y, 0.f), -90.f));
	}
	for (int j = 0; j < 4; ++j) {
		std::vector<glm::vec3> lvl;
		for (int i = 0; i < 4; ++i) {
			lvl.push_back(glm::rotateY(pts[i] / 2.f, startAngle + (30.f) * j));
		}
		grid.push_back(lvl);
	}
}

void Bezier2D::makeVertices(int curveIndex, int& startIndex, IndexedModel& model)
{ // Generate Vertices Grid
	glm::vec3 color(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++) { //For Every Start Angle (0,90,180,270)
		std::vector<std::vector<int>> ind_indexes;
		std::vector<std::vector<glm::vec3>> grid;
		make2DCtrlPoints(curveIndex, (float)i * 90.f, grid);
		for (int u = 0; u < 20; ++u) { // For Every U (0.0 -> 1.0) Horizontal
			std::vector<int> vec;
			for (int v = 0; v < 20; ++v) { // For Every V (0.0 -> 1.0) Vertical
				vec.push_back(startIndex++);
				model.positions.push_back(evalBezierPatch(grid, (float)u * 0.05f, (float)v * 0.05f));
				model.normals.push_back(crossDeriv(grid, (float)u * 0.05f, (float)v * 0.05f));
				model.texCoords.push_back(glm::vec2(u * 0.05f, v * 0.05f));
				model.colors.push_back(color);
			}
			ind_indexes.push_back(vec);
		}
		// Put Indexes in the indecis array
		for (int u = 0; u < 19; ++u) {
			for (int v = 0; v < 19; ++v) {
				model.indices.push_back(ind_indexes[u][v]);
				model.indices.push_back(ind_indexes[u][v + 1]);
				model.indices.push_back(ind_indexes[u + 1][v]);

				model.indices.push_back(ind_indexes[u][v + 1]);
				model.indices.push_back(ind_indexes[u + 1][v]);
				model.indices.push_back(ind_indexes[u + 1][v + 1]);
			}
		}
	}

}


IndexedModel Bezier2D::GetSurface()
{
	//makeVertices
	IndexedModel model;
	int cnt = 0;
	for (int i = 0; i < segNum; ++i) {
		makeVertices(i, cnt, model);
	}
	//Connect Edges
	for (int i = 0; i < segNum; i++) { 
		//TODO - FIX THAT!
		for (int k = 19 + 1600 * i, m = CALC_KOFFSET((1600 + 1600 * i), i); k < 1599 + 1600 * i; k += 20, m += 20) {
			model.indices.push_back(k);
			model.indices.push_back(k + 20);
			model.indices.push_back(m + 20);

			model.indices.push_back(m + 20);
			model.indices.push_back(k);
			model.indices.push_back(m);
		}

		for (int k = 0; k < 4; k++) {
			for (int m = (380 + 1600 * i + 400 * k); m < (399 + 1600 * i + 400 * k); m++) {
				model.indices.push_back(m);
				model.indices.push_back(CALC_OFFSET(m + 20, i));
				model.indices.push_back(m + 1);

				model.indices.push_back(CALC_OFFSET(m + 20, i));
				model.indices.push_back(CALC_OFFSET(m + 21, i));
				model.indices.push_back(m + 1);
			}
		}
	}

	return model;
}

Bezier2D::~Bezier2D(void)
{

}