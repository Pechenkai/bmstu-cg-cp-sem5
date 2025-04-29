#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;

    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec3& norm)
        : position(pos), normal(norm) {}
};



#endif //VERTEX_H
