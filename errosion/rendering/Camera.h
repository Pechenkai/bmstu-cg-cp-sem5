#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(float aspectRatio);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    void setAspectRatio(float aspectRatio);
    void setPosition(glm::vec3 position);
    void setTarget(glm::vec3 target);
    void setUp(glm::vec3 up);
    glm::vec3 getPosition() const;
    void updatePosition(float pitch, float yaw, float rad);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    float m_pitchAngle;
    float m_yawAngle;
    float m_radius;
};


#endif //CAMERA_H
