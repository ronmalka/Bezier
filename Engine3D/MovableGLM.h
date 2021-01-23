#pragma once
#include "glm/glm.hpp"

class MovableGLM
{
public:
	MovableGLM();
	glm::mat4 MakeTrans(const glm::mat4& prevTransformations) const;
	glm::mat4 MakeTrans() const;
	void MyTranslate(const glm::vec3 delta, int mode);
	void MyRotate(float angle, const glm::vec3& vec, int mode);
	void MyScale(const glm::vec3 scale);
	void ZeroTrans();
	void MoveAround(float angle, const glm::vec3& vec);
	inline glm::mat4 getTrans() { return trans; }
	inline glm::mat4 getRot() { return rot; }
	inline glm::mat4 getScl() { return scl; }
private:
	glm::mat4 trans;
	glm::mat4 rot;
	glm::mat4 scl;
};
