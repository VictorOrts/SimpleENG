#pragma once
#include "deps/glm/glm.hpp"

class PointLight
{
public:
	PointLight();
	~PointLight();

	void setPosition(glm::vec3 newpos);
	glm::vec3 getPosition();
private:
	glm::vec3 position;
};

PointLight::PointLight()
{
}

PointLight::~PointLight()
{
}

void PointLight::setPosition(glm::vec3 newpos) {
	position = newpos;
}

glm::vec3 PointLight::getPosition() {
	return position;
}