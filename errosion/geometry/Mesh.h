#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Vertex.h"
#include "Face.h"
#include "../terrain/TerrainGenerator.h"

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void generateMesh(const TerrainGenerator& terrain);
    void calculateNormals();

    const std::vector<Vertex>& getVertices() const;
    const std::vector<Face>& getFaces() const;
    void setVertices(const std::vector<Vertex>& vertices);
    void setFaces(const std::vector<Face>& faces);

private:
    std::vector<Vertex> m_vertices;
    std::vector<Face> m_faces;
    glm::vec3 checkPoint;
};

#endif // MESH_H
