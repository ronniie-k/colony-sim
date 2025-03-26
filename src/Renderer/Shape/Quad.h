#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Quad
{
public:
	Quad(const glm::vec2& position = { 0, 0 }, float rotation = 0, const glm::vec2& scale = { 1, 1 });

	void setColor(const glm::vec3& color) { m_color = color; }
	void setPosition(const glm::vec2& position) { m_position = position; }
	void setScale(const glm::vec2& scale) { m_scale = scale; }
	void setRotation(float angle) { m_rotation = angle; }

	void move(const glm::vec2& delta);

private:
	glm::vec2 m_position;
	glm::vec2 m_scale;
	float m_rotation;
	glm::vec3 m_color = { 1, 1, 1 };
};