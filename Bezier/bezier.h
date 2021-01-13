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
	void setNewOffset(double x, double y);
	void setNewOffsetWithRotate(float x, float y);
	void setNewOffsetWithChilds(double x, double y);
	unsigned int TextureDesine(int width, int height);
	~bezier(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }
	inline Bezier1D* get1D() { return bezier1D; }
	void UpdatePosition( float xpos, float ypos);
	std::vector<int> FindChilds(int parent);
	void AlignPoints();
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float old_x;
	float old_y;
	float offset_x;
	float offset_y;
	bool new_radius;

};

