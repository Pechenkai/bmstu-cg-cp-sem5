#include "Mesh.h"
#include <iostream>
#include <glm/glm.hpp>

Mesh::Mesh() = default;

Mesh::~Mesh() = default;

const std::vector<Face>& Mesh::getFaces() const
{
    return m_faces;
}

const std::vector<Vertex>& Mesh::getVertices() const
{
    return m_vertices;
}

void Mesh::generateMesh(const TerrainGenerator& terrain)
{
    int width = terrain.getWidth();
    int height = terrain.getHeight();
    const auto& heightMap = terrain.getHeightMap();

    checkPoint = glm::vec3(0, 50, 0);

    m_faces.clear();
    m_vertices.clear();

    for (int z = 0; z < height; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            float y = heightMap[x][z];
            Vertex vertex;
            float halfWidth = (width - 1) / 2.0f;
            float halfHeight = (height - 1) / 2.0f;
            vertex.position = glm::vec3(static_cast<float>(x - halfWidth), y, static_cast<float>(z - halfHeight));
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            m_vertices.push_back(vertex);
        }
    }

    for (int z = 0; z < height - 1; ++z)
    {
        for (int x = 0; x < width - 1; ++x)
        {
            unsigned int topLeft = x + z * width;
            unsigned int topRight = (x + 1) + z * width;
            unsigned int bottomLeft = x + (z + 1) * width;
            unsigned int bottomRight = (x + 1) + (z + 1) * width;

            m_faces.emplace_back(topLeft, bottomLeft, topRight);

            m_faces.emplace_back(topRight, bottomLeft, bottomRight);
        }
    }

    calculateNormals();
}

void Mesh::calculateNormals()
{
    for (auto& vertex : m_vertices)
    {
        vertex.normal = glm::vec3(0.0f);
    }

    for (auto& face : m_faces)
    {
        unsigned int idx0 = face.indices[0];
        unsigned int idx1 = face.indices[1];
        unsigned int idx2 = face.indices[2];

        const glm::vec3& v0 = m_vertices[idx0].position;
        const glm::vec3& v1 = m_vertices[idx1].position;
        const glm::vec3& v2 = m_vertices[idx2].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 faceNormal = glm::cross(edge1, edge2);
        faceNormal = glm::normalize(faceNormal);

        glm::vec3 toReference = v0 - checkPoint;
        if (glm::dot(faceNormal, toReference) > 0.0f)
        {
            faceNormal = -faceNormal;
        }

        face.m_normal = faceNormal;

        m_vertices[idx0].normal += faceNormal;
        m_vertices[idx1].normal += faceNormal;
        m_vertices[idx2].normal += faceNormal;
    }

    for (auto& vertex : m_vertices)
    {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

void Mesh::setFaces(const std::vector<Face>& faces)
{
    m_faces = faces;
}

void Mesh::setVertices(const std::vector<Vertex>& vertices)
{
    m_vertices = vertices;
}
