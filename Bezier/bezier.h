#pragma once
#include "scene.h"
#include "glm/gtx/rotate_vector.hpp"


class bezier : public Scene
{
public:
	
	bezier();
	void Init();
	void Update(const glm::mat4& Projection, const glm::mat4& View, const glm::mat4& Model, const int shaderIndx);	
	void WhenRotate();
	void WhenTranslate();
	void WhenPicked();
	void Motion();
	void updatePressedPos(double xpos, double ypos);
	void pickedMove(double xpos, double ypos, float zoomCo);
	void setNewOffsetWithRotate(float x, float y);
	void setNewOffsetWithChilds(double x, double y);
	void setNewOffsetWithConvex(double x, double y, int strartPoint);
	unsigned int TextureDesine(int width, int height);
	~bezier(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }
	inline Bezier1D* get1D() { return bezier1D; }
	void UpdatePosition( float xpos, float ypos);
	void moveXYZ(glm::vec4 objLoc, int dir);
	void setNewOffset(double xpos, double ypos, bool is3D, bool isRotate, float zoomCo);
	std::vector<int> FindChilds(int parent);
	void AlignPoints();
	int xPos, yPos;
	int xWhenBlend, yWhenBlend;
	int toUpdatePicked = 0;
	
	virtual void Draw(int shaderIndx, const glm::mat4& Projection, glm::mat4& View, int viewportIndx, unsigned int flags);
	int HandleConvexHull(float xpos, float ypos, bool btn_left);
	inline bool getCont() { return cont; }
	inline void setCont() { cont = !cont; }
	inline void setAngel(bool angel) { new_angel = angel; }
	void movePointWithAngel(float parentX, float parentY, float angle);

	
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float old_x;
	float old_y;
	float offset_x;
	float offset_y;
	bool new_angel;
	bool cont = false;


};

