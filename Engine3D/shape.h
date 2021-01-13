#pragma once
#include "MovableGLM.h"
#include "VertexArray.hpp"
#include "shader.h"
#include "MeshConstructor.h" 

class Shape : public MovableGLM
{
private:
	
	unsigned int materialID;
	int shaderID;
	bool isCopy;
	unsigned int viewports;

public:

	Shape(const Shape& shape, unsigned int mode);

	Shape(const std::string& fileName, unsigned int mode);

	Shape(const int SimpleShapeType, unsigned int mode);

	Shape(int segNum, int mode, std::vector<glm::vec3> controlPoints, int viewport);

	void Draw(const Shader* shaders, bool isPicking);

	inline void AddViewport(int viewport) { viewports = viewports | (1<<viewport); }

	inline void RemoveViewport(int viewport) { viewports = viewports & ~(1 << viewport); }

	inline bool Is2Render(int viewport) { return  viewports & (1 << viewport); }

	inline void SetShader(const int id) { shaderID = id; }

	inline int GetShader() { return shaderID; }

	inline void SetMaterial(const unsigned int id) { materialID = id; }

	inline unsigned int GetMaterial() { return materialID; }

	inline std::vector<glm::vec3>& GetControlPoints() { return controlPoints; }
	inline void setRadius(float radius) { this->radius = radius; }
	inline float getRadius() { return this->radius; }
	virtual ~Shape(void);
	Shape(int segNum, std::vector<glm::vec3> controlPoints, int mode, int viewport);
protected:
	std::vector<glm::vec3> controlPoints;
	int segNum;
	unsigned int mode;
	MeshConstructor* mesh;
	float radius = -1.; // PI/2 limit of arctan
};


