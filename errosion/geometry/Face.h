#ifndef FACE_H
#define FACE_H

#include <glm/glm.hpp>

struct Face
{
    unsigned int indices[3];
    glm::vec3 m_normal;

    Face() = default;
    Face(unsigned int i0, unsigned int i1, unsigned int i2)
    {
        indices[0] = i0;
        indices[1] = i1;
        indices[2] = i2;

        m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

#endif
