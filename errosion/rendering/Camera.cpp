#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float aspectRatio)
    : m_position(40.0f, -10.0f, 40.0f),
      m_target(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_fov(glm::radians(45.0f)),
      m_aspectRatio(aspectRatio),
      m_nearPlane(10.0f),
      m_farPlane(100.0f),
      m_radius(50),
      m_pitchAngle(0),
      m_yawAngle(0)

{
}

void Camera::setAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::setPosition(glm::vec3 position)
{
    m_position = position;
}

void Camera::setTarget(glm::vec3 target)
{
    m_target = target;
}

void Camera::setUp(glm::vec3 up)
{
    m_up = up;
}

glm::vec3 Camera::getPosition() const
{
    return m_position;
}

void Camera::updatePosition(float pitch, float yaw, float rad)
{
    m_pitchAngle += pitch;
    m_yawAngle += yaw;

    if (rad < 0 && m_radius + rad > 10 || rad > 0)
        m_radius += rad;

    float x = m_radius * cos(m_pitchAngle) * sin(m_yawAngle);
    float y = m_radius * sin(m_pitchAngle);
    float z = m_radius * cos(m_pitchAngle) * cos(m_yawAngle);

    m_position = glm::vec3(x, y, z);
}
