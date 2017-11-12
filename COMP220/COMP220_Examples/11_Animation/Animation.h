#pragma once

#include <string>
#include <vector>
#include <map>

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

struct AnimationClip
{
	std::string Name;
	std::vector<glm::mat4> transformations;
};

class Animations
{
public:
	Animations();
	~Animations();

private:
	std::map<std::string, AnimationClip*> m_AnimationClips;
};