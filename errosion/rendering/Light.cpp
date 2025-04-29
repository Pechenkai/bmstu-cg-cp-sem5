#include "Light.h"

Light::Light(const glm::vec3& position, const glm::vec3& color)
    : m_position(position), m_color(color), m_abs_color(color), m_intensity(10.0f)
{
}

Light::~Light() = default;

void Light::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void Light::setColor(const glm::vec3& color)
{
    m_abs_color = color;
    m_color = m_abs_color * (m_intensity / 10.f);
}

glm::vec3 Light::getPosition() const
{
    return m_position;
}

glm::vec3 Light::getColor() const
{
    return m_color;
}

void Light::setIntensity(float intensity)
{
    m_intensity = intensity;
    m_color = m_abs_color * (intensity / 10.f);
}
