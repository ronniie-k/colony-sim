#include "Quad.h"


Quad::Quad(const glm::vec2& position, float rotation, const glm::vec2& scale)
{
}

void Quad::move(const glm::vec2& delta)
{
	m_position += delta;
}
