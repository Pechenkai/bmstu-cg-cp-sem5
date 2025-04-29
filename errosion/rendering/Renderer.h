#ifndef RENDERER_H
#define RENDERER_H

#include "../geometry/Mesh.h"
#include "Light.h"
#include "Camera.h"
#include <QImage>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>

class Renderer
{
public:
    Renderer(int width, int height, float minHeight, float maxHeight);
    ~Renderer();

    void clearBuffers();
    void render(const Mesh& mesh, const Camera& camera, const Light& light);
    QImage getFrameBuffer() const;
    void setModelMatrix(const glm::mat4& modelMatrix);
    void resize(int width, int height);
    void setMinMaxHeight(float minh, float maxh);

private:
    float minHeight;
    float maxHeight;
    void drawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Light& light,
                      const glm::mat4& mvpMatrix);
    void drawTrianglePhong(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                               const Light& light, const glm::mat4& mvpMatrix);
    void drawTriangleGuro(const Vertex& v0,
                                      const Vertex& v1,
                                      const Vertex& v2,
                                      const Light& light,
                                      const glm::mat4& mvpMatrix);
    glm::vec3 getColorByHeight(float height) const;
    bool updateZBuffer(int x, int y, float depth);

    int m_width;
    int m_height;
    std::vector<float> m_zBuffer;
    QImage m_frameBuffer;


    glm::mat4 m_modelMatrix;
    std::mutex frameBufferMutex;
    std::mutex zBufferMutex;
};


#endif //RENDERER_H
