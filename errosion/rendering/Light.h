#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    Light(const glm::vec3& position, const glm::vec3& color);
    ~Light();

    void setPosition(const glm::vec3& position);
    void setColor(const glm::vec3& color);
    void setIntensity(float intensity);

    glm::vec3 getPosition() const;
    glm::vec3 getColor() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_abs_color;
    glm::vec3 m_color;
    int m_intensity;
};



#endif //LIGHT_H
