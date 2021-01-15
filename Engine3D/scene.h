#pragma once
#include "shader.h"
#include "camera.h"
#include "VertexArray.hpp"
#include "Material.h"
#include <vector>
#include "Bezier/Bezier1D.h"
#include "Bezier/Bezier2D.h"


class Scene : public MovableGLM
{

public:
	enum axis { xAxis, yAxis, zAxis };
	enum transformations { xTranslate, yTranslate, zTranslate, xRotate, yRotate, zRotate, xScale, yScale, zScale, xCameraTranslate, yCameraTranslate, zCameraTranslate,xMyRotate,yMyRotate,zMyRotate };
	enum modes { POINTS, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS };
	enum shapes { Axis, Plane, Cube, Octahedron, Tethrahedron, LineCopy, MeshCopy, Curve, Surface };
	enum buffers { COLOR, DEPTH, STENCIL, BACK, FRONT, NONE };

	Scene();
	//Scene(float angle, float relationWH, float near, float far);

	void AddShapeFromFile(const std::string& fileName, int parent, unsigned int mode);
	virtual void AddShape(int type, int parent, unsigned int mode);
	virtual void AddShape(int segNum, int parent, unsigned int mode, std::vector<glm::vec3> controlPoints);
	virtual void AddShape(int segNum, std::vector<glm::vec3> controlPoints, int parent, unsigned int mode);
	void AddShapeCopy(int indx, int parent, unsigned int mode);

	int AddShader(const std::string& fileName);
	int AddTexture(const std::string& textureFileName, int dim);
	int AddTexture(int width, int height, unsigned char* data, int mode);
	int AddMaterial(unsigned int texIndices[], unsigned int slots[], unsigned int size);
	void ZeroShapesTrans();

	virtual void Update(const glm::mat4& Projection, const glm::mat4& View, const glm::mat4& Model, const int shaderIndx) = 0;
	virtual void WhenTranslate() {};
	virtual void WhenRotate() {};
	virtual void WhenPicked() {};
	virtual void Motion() {};
	virtual void Reset() {};
	virtual void Draw(int shaderIndx, const glm::mat4& Projection, glm::mat4& View, int viewportIndx, unsigned int flags);
	virtual ~Scene(void);

	void ShapeTransformation(int type, float amt);

	bool Picking(unsigned char data[4]);

	inline void SetParent(int indx, int newValue) { chainParents[indx] = newValue; }

	void ReadPixel();

	inline void Activate() { isActive = true; }
	inline void Deactivate() { isActive = false; }
	void AddShapeViewport(int shpIndx, int viewportIndx);
	void RemoveShapeViewport(int shpIndx, int viewportIndx);
	void BindMaterial(Shader* s, unsigned int materialIndx);
	void BindTexture(int texIndx, int slot) { textures[texIndx]->Bind(slot); }

	void MouseProccessing(int button, int xrel, int yrel);
	bool inline IsActive() const { return isActive; }

	inline void SetShapeMaterial(int shpIndx, int materialIndx) { shapes[shpIndx]->SetMaterial(materialIndx); }
	inline void SetShapeShader(int shpIndx, int shdrIndx) { shapes[shpIndx]->SetShader(shdrIndx); }

	inline void SetPickedShape(int pick) { pickedShape = pick; }
	inline int GetPickedShape() { return pickedShape; }

	inline Bezier1D* GetBezier1D() { return bezier1D; }

	inline void ZeroShapeTrans(int shpIndx) { shapes[shpIndx]->ZeroTrans(); }
	inline int getShapeSize() { return shapes.size(); }
	bool drawPlane = true;

private:
	float depth;

protected:
	std::vector<Shape*> shapes;
	std::vector<Shader*> shaders;
	std::vector<int> chainParents;
	std::vector<Texture*> textures;
	std::vector<Material*> materials;

	int pickedShape;

	bool isActive;

	Bezier1D* bezier1D;
};

