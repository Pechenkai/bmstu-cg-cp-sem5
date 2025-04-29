#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <QDebug>
#include <omp.h>

#include "Renderer.h"
#include <limits>

int CH_COLOR = 0;

Renderer::Renderer(int width, int height, float minHeight, float maxHeight)
    : m_width(width), m_height(height), m_frameBuffer(width, height, QImage::Format_RGB32),
      minHeight(minHeight), maxHeight(maxHeight), m_modelMatrix(glm::mat4(1.0f))
{
    m_zBuffer.resize(width * height, std::numeric_limits<float>::infinity());
}

void Renderer::setModelMatrix(const glm::mat4& modelMatrix)
{
    m_modelMatrix = modelMatrix;
}

void Renderer::resize(int width, int height)
{
    m_width = width;
    m_height = height;

    m_frameBuffer = QImage(width, height, QImage::Format_RGB32);
    m_zBuffer.resize(width * height, std::numeric_limits<float>::infinity());
}

Renderer::~Renderer() = default;

void Renderer::clearBuffers()
{
    std::fill(m_zBuffer.begin(), m_zBuffer.end(), std::numeric_limits<float>::infinity());

    m_frameBuffer.fill(Qt::black);
}

QImage Renderer::getFrameBuffer() const
{
    return m_frameBuffer;
}

void Renderer::render(const Mesh& mesh, const Camera& camera, const Light& light)
{
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix;

    const auto& faces = mesh.getFaces();
    const auto& vertices = mesh.getVertices();

    int numThreads = 8;
    size_t numFaces = faces.size();


#pragma omp parallel num_threads(numThreads)
    {
        int threadId = omp_get_thread_num();
        size_t facesPerThread = (numFaces + numThreads - 1) / numThreads;
        size_t startIdx = threadId * facesPerThread;
        size_t endIdx = std::min(startIdx + facesPerThread, numFaces);

        for (size_t i = startIdx; i < endIdx; ++i)
        {
            const auto& face = faces[i];
            const Vertex& v0 = vertices[face.indices[0]];
            const Vertex& v1 = vertices[face.indices[1]];
            const Vertex& v2 = vertices[face.indices[2]];

            drawTriangleGuro(v0, v1, v2, light, mvpMatrix);
        }
    }
}

void Renderer::drawTrianglePhong(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                                     const Light& light, const glm::mat4& mvpMatrix)
{
    glm::vec4 p0_clip = mvpMatrix * glm::vec4(v0.position, 1.0f);
    glm::vec4 p1_clip = mvpMatrix * glm::vec4(v1.position, 1.0f);
    glm::vec4 p2_clip = mvpMatrix * glm::vec4(v2.position, 1.0f);

    float w0 = p0_clip.w;
    float w1 = p1_clip.w;
    float w2 = p2_clip.w;

    p0_clip /= w0;
    p1_clip /= w1;
    p2_clip /= w2;

    auto ndcToScreen = [&](const glm::vec4& p)
    {
        float x = (p.x * 0.5f + 0.5f) * m_width;
        float y = (1.0f - (p.y * 0.5f + 0.5f)) * m_height;
        float z = p.z;
        return glm::vec3(x, y, z);
    };

    glm::vec3 p0 = ndcToScreen(p0_clip);
    glm::vec3 p1 = ndcToScreen(p1_clip);
    glm::vec3 p2 = ndcToScreen(p2_clip);

    float minX = std::floor(std::min({p0.x, p1.x, p2.x}));
    float maxX = std::ceil(std::max({p0.x, p1.x, p2.x}));
    float minY = std::floor(std::min({p0.y, p1.y, p2.y}));
    float maxY = std::ceil(std::max({p0.y, p1.y, p2.y}));

    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min((float)m_width - 1, maxX);
    maxY = std::min((float)m_height - 1, maxY);

    auto edgeFunction = [](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };

    float area = edgeFunction(p0, p1, p2);
    if (area == 0.0f) return;

    glm::vec3 lightPos = light.getPosition();
    glm::vec3 lightColor = light.getColor();

    glm::vec3 pos0_over_w = v0.position / w0;
    glm::vec3 pos1_over_w = v1.position / w1;
    glm::vec3 pos2_over_w = v2.position / w2;

    glm::vec3 norm0_over_w = v0.normal / w0;
    glm::vec3 norm1_over_w = v1.normal / w1;
    glm::vec3 norm2_over_w = v2.normal / w2;

    for (int y = (int)minY; y <= (int)maxY; y++)
    {
        for (int x = (int)minX; x <= (int)maxX; x++)
        {
            glm::vec3 p((float)x + 0.5f, (float)y + 0.5f, 0.0f);
            float wA = edgeFunction(p1, p2, p);
            float wB = edgeFunction(p2, p0, p);
            float wC = edgeFunction(p0, p1, p);

            if (area > 0 && wA > 0 && wB > 0 && wC > 0 || area <= 0 && wA <= 0 && wB <= 0 && wC <= 0)
            {
                float lambda0 = wA / area;
                float lambda1 = wB / area;
                float lambda2 = wC / area;

                float invW = (lambda0 / w0) + (lambda1 / w1) + (lambda2 / w2);

                glm::vec3 fragPos = (pos0_over_w * lambda0 + pos1_over_w * lambda1 + pos2_over_w * lambda2) / invW;
                glm::vec3 fragNormal = (norm0_over_w * lambda0 + norm1_over_w * lambda1 + norm2_over_w * lambda2) /
                    invW;
                fragNormal = glm::normalize(fragNormal);

                float z = (p0.z * lambda0 + p1.z * lambda1 + p2.z * lambda2);

                int index = x + y * m_width;

                zBufferMutex.lock();
                float currentDepth = m_zBuffer[index];
                if (z < currentDepth)
                {
                    m_zBuffer[index] = z;
                    zBufferMutex.unlock();

                    glm::vec3 toLight = glm::normalize(lightPos - fragPos);
                    float lambert = glm::max(glm::dot(fragNormal, toLight), 0.0f);
                    glm::vec3 worldPos = lambda0 * v0.position + lambda1 * v1.position + lambda2 * v2.position;

                    glm::vec3 colorByHeight = getColorByHeight(worldPos.y);
                    glm::vec3 fragColor = colorByHeight * lightColor * lambert;

                    frameBufferMutex.lock();
                    m_frameBuffer.setPixelColor(x, y, QColor(
                                                    static_cast<int>(fragColor.r * 255),
                                                    static_cast<int>(fragColor.g * 255),
                                                    static_cast<int>(fragColor.b * 255)
                                                ));

                    frameBufferMutex.unlock();
                }
                else
                    zBufferMutex.unlock();
            }
        }
    }
}

void Renderer::drawTriangleGuro(const Vertex& v0,
                                            const Vertex& v1,
                                            const Vertex& v2,
                                            const Light& light,
                                            const glm::mat4& mvpMatrix)
{
    glm::vec4 p0_clip = mvpMatrix * glm::vec4(v0.position, 1.0f);
    glm::vec4 p1_clip = mvpMatrix * glm::vec4(v1.position, 1.0f);
    glm::vec4 p2_clip = mvpMatrix * glm::vec4(v2.position, 1.0f);

    float w0 = p0_clip.w;
    float w1 = p1_clip.w;
    float w2 = p2_clip.w;

    p0_clip /= w0;
    p1_clip /= w1;
    p2_clip /= w2;

    auto ndcToScreen = [&](const glm::vec4& p)
    {
        float x = (p.x * 0.5f + 0.5f) * m_width;
        float y = (1.0f - (p.y * 0.5f + 0.5f)) * m_height;
        float z = p.z;
        return glm::vec3(x, y, z);
    };

    glm::vec3 p0 = ndcToScreen(p0_clip);
    glm::vec3 p1 = ndcToScreen(p1_clip);
    glm::vec3 p2 = ndcToScreen(p2_clip);

    auto computeLambertIntensity = [&](const glm::vec3& worldPos, const glm::vec3& normal)
    {
        glm::vec3 toLight = glm::normalize(light.getPosition() - worldPos);
        float lambert = glm::max(glm::dot(glm::normalize(normal), toLight), 0.0f);
        return lambert;
    };

    float intensity0 = computeLambertIntensity(v0.position, v0.normal);
    float intensity1 = computeLambertIntensity(v1.position, v1.normal);
    float intensity2 = computeLambertIntensity(v2.position, v2.normal);

    float intens0_over_w = intensity0 / w0;
    float intens1_over_w = intensity1 / w1;
    float intens2_over_w = intensity2 / w2;

    glm::vec3 pos0_over_w = v0.position / w0;
    glm::vec3 pos1_over_w = v1.position / w1;
    glm::vec3 pos2_over_w = v2.position / w2;

    float minX = std::floor(std::min({p0.x, p1.x, p2.x}));
    float maxX = std::ceil(std::max({p0.x, p1.x, p2.x}));
    float minY = std::floor(std::min({p0.y, p1.y, p2.y}));
    float maxY = std::ceil(std::max({p0.y, p1.y, p2.y}));

    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min((float)m_width - 1, maxX);
    maxY = std::min((float)m_height - 1, maxY);

    auto edgeFunction = [](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };

    float area = edgeFunction(p0, p1, p2);
    if (area == 0.0f) return;

    for (int y = (int)minY; y <= (int)maxY; y++)
    {
        for (int x = (int)minX; x <= (int)maxX; x++)
        {
            glm::vec3 p((float)x + 0.5f, (float)y + 0.5f, 0.0f);

            float wA = edgeFunction(p1, p2, p);
            float wB = edgeFunction(p2, p0, p);
            float wC = edgeFunction(p0, p1, p);

            if ((area > 0 && wA > 0 && wB > 0 && wC > 0) ||
                (area < 0 && wA < 0 && wB < 0 && wC < 0))
            {
                float lambda0 = wA / area;
                float lambda1 = wB / area;
                float lambda2 = wC / area;

                float invW = (lambda0 / w0) + (lambda1 / w1) + (lambda2 / w2);
                float fragIntensity =
                    (intens0_over_w * lambda0 +
                     intens1_over_w * lambda1 +
                     intens2_over_w * lambda2) / invW;

                glm::vec3 fragPos =
                    (pos0_over_w * lambda0 +
                     pos1_over_w * lambda1 +
                     pos2_over_w * lambda2) / invW;

                float z = (p0.z * lambda0 + p1.z * lambda1 + p2.z * lambda2);

                int index = x + y * m_width;
                zBufferMutex.lock();
                float currentDepth = m_zBuffer[index];
                if (z < currentDepth)
                {
                    m_zBuffer[index] = z;
                    zBufferMutex.unlock();

                    glm::vec3 colorByHeight = getColorByHeight(fragPos.y);
                    glm::vec3 lightColor = light.getColor();

                    glm::vec3 fragColor = colorByHeight * lightColor * fragIntensity;

                    frameBufferMutex.lock();
                    m_frameBuffer.setPixelColor(x, y, QColor(
                        static_cast<int>(fragColor.r * 255),
                        static_cast<int>(fragColor.g * 255),
                        static_cast<int>(fragColor.b * 255)
                    ));
                    frameBufferMutex.unlock();
                }
                else
                    zBufferMutex.unlock();
            }
        }
    }
}

glm::vec3 Renderer::getColorByHeight(float height) const
{
    float normalizedHeight = (height - minHeight) / (maxHeight - minHeight);
    normalizedHeight = glm::clamp(normalizedHeight, 0.0f, 1.0f);

    glm::vec3 brownColor(0.545f, 0.27f, 0.07f);
    glm::vec3 greenColor(0.0f, 0.5f, 0.0f);
    glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

    float lowerThreshold = 0.93f;

    if (normalizedHeight < lowerThreshold)
    {
        float t = normalizedHeight / lowerThreshold;
        return glm::mix(brownColor, greenColor, t);
    }
    else
    {
        float t = (normalizedHeight - lowerThreshold) / (1.0f - lowerThreshold);
        return glm::mix(greenColor, whiteColor, t);
    }
}


bool Renderer::updateZBuffer(int x, int y, float depth)
{
    int index = x + y * m_width;
    if (depth < m_zBuffer[index])
    {
        m_zBuffer[index] = depth;
        return true;
    }
    return false;
}

void Renderer::drawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                            const Light& light, const glm::mat4& mvpMatrix)
{
    glm::vec4 p0_clip = mvpMatrix * glm::vec4(v0.position, 1.0f);
    glm::vec4 p1_clip = mvpMatrix * glm::vec4(v1.position, 1.0f);
    glm::vec4 p2_clip = mvpMatrix * glm::vec4(v2.position, 1.0f);

    float w0 = p0_clip.w;
    float w1 = p1_clip.w;
    float w2 = p2_clip.w;

    p0_clip /= w0;
    p1_clip /= w1;
    p2_clip /= w2;

    auto ndcToScreen = [&](const glm::vec4& p)
    {
        float x = (p.x * 0.5f + 0.5f) * m_width;
        float y = (p.y * 0.5f + 0.5f) * m_height;
        float z = p.z;
        return glm::vec3(x, y, z);
    };

    glm::vec3 p0 = ndcToScreen(p0_clip);
    glm::vec3 p1 = ndcToScreen(p1_clip);
    glm::vec3 p2 = ndcToScreen(p2_clip);

    float minX = std::floor(std::min({p0.x, p1.x, p2.x}));
    float maxX = std::ceil(std::max({p0.x, p1.x, p2.x}));
    float minY = std::floor(std::min({p0.y, p1.y, p2.y}));
    float maxY = std::ceil(std::max({p0.y, p1.y, p2.y}));

    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min((float)m_width - 1, maxX);
    maxY = std::min((float)m_height - 1, maxY);

    auto edgeFunction = [](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };

    float area = edgeFunction(p0, p1, p2);
    if (area == 0.0f) return;

    glm::vec3 lightPos = light.getPosition();
    glm::vec3 lightColor = light.getColor();

    glm::vec3 pos0_over_w = v0.position / w0;
    glm::vec3 pos1_over_w = v1.position / w1;
    glm::vec3 pos2_over_w = v2.position / w2;

    glm::vec3 norm0_over_w = v0.normal / w0;
    glm::vec3 norm1_over_w = v1.normal / w1;
    glm::vec3 norm2_over_w = v2.normal / w2;

    for (int y = (int)minY; y <= (int)maxY; y++)
    {
        for (int x = (int)minX; x <= (int)maxX; x++)
        {
            glm::vec3 p((float)x + 0.5f, (float)y + 0.5f, 0.0f);
            float wA = edgeFunction(p1, p2, p);
            float wB = edgeFunction(p2, p0, p);
            float wC = edgeFunction(p0, p1, p);

            if (area > 0 && wA > 0 && wB > 0 && wC > 0 || area <= 0 && wA <= 0 && wB <= 0 && wC <= 0)
            {
                float lambda0 = wA / area;
                float lambda1 = wB / area;
                float lambda2 = wC / area;

                float invW = (lambda0 / w0) + (lambda1 / w1) + (lambda2 / w2);

                glm::vec3 fragPos = (pos0_over_w * lambda0 + pos1_over_w * lambda1 + pos2_over_w * lambda2) / invW;
                glm::vec3 fragNormal = (norm0_over_w * lambda0 + norm1_over_w * lambda1 + norm2_over_w * lambda2) /
                    invW;
                fragNormal = glm::normalize(fragNormal);

                float z = (p0.z * lambda0 + p1.z * lambda1 + p2.z * lambda2);

                int index = x + y * m_width;
                float currentDepth = m_zBuffer[index];
                if (z < currentDepth)
                {
                    glm::vec3 toLight = glm::normalize(lightPos - fragPos);
                    float lambert = glm::max(glm::dot(fragNormal, toLight), 0.0f);
                    glm::vec3 worldPos = lambda0 * v0.position + lambda1 * v1.position + lambda2 * v2.position;

                    glm::vec3 colorByHeight = getColorByHeight(-worldPos.y);
                    glm::vec3 fragColor = colorByHeight * lightColor * lambert;

                    m_frameBuffer.setPixelColor(x, y, QColor(
                                                    static_cast<int>(fragColor.r * 255),
                                                    static_cast<int>(fragColor.g * 255),
                                                    static_cast<int>(fragColor.b * 255)
                                                ));

                    m_zBuffer[index] = z;
                }
            }
        }
    }
}

void Renderer::setMinMaxHeight(float minh, float maxh)
{
    maxHeight = maxh;
    minHeight = minh;
}
