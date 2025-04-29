#include "TerrainGenerator.h"
#include <limits>
#include <algorithm>
#include <cmath>

TerrainGenerator::TerrainGenerator(int width, int height)
    : m_width(width), m_height(height), m_scale(1.0f), m_noiseGenerator(nullptr)
{
    m_heightMap.resize(m_width, std::vector<float>(m_height, 0.0f));
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::calculateScale()
{
    m_scale = std::sqrt(m_height * m_width) / 30 * 2.5;
}


void TerrainGenerator::setNoiseGenerator(NoiseGenerator* generator)
{
    m_noiseGenerator = generator;
}

void TerrainGenerator::setScale(float scale)
{
    m_scale = scale;
}

void TerrainGenerator::generateHeightMap()
{
    if (!m_noiseGenerator)
    {
        std::cerr << "Noise generator is not set!" << std::endl;
        return;
    }

    calculateScale();

    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            float nx = static_cast<float>(x) / (m_width - 1);
            float ny = static_cast<float>(y) / (m_height - 1);
            nx *= m_scale;
            ny *= m_scale;

            float noiseValue = m_noiseGenerator->generateNoise(nx, ny);
            m_heightMap[x][y] = noiseValue;
        }
    }
}


const std::vector<std::vector<float>> &TerrainGenerator::getHeightMap() const
{
    return m_heightMap;
}

int TerrainGenerator::getWidth() const
{
    return m_width;
}

int TerrainGenerator::getHeight() const
{
    return m_height;
}

void TerrainGenerator::setWidth(int width)
{
    m_width = width;
}

void TerrainGenerator::setHeight(int height)
{
    m_height = height;
}


float TerrainGenerator::getMinHeight() const
{
    float minVal = std::numeric_limits<float>::max();
    for (const auto& row : m_heightMap)
    {
        float rowMin = *std::min_element(row.begin(), row.end());
        if (rowMin < minVal)
            minVal = rowMin;
    }
    return minVal;
}

float TerrainGenerator::getMaxHeight() const
{
    float maxVal = std::numeric_limits<float>::lowest();
    for (const auto& row : m_heightMap)
    {
        float rowMax = *std::max_element(row.begin(), row.end());
        if (rowMax > maxVal)
            maxVal = rowMax;
    }
    return maxVal;
}

void TerrainGenerator::setHeightMap(const std::vector<std::vector<float>> &heightMap)
{
    m_heightMap = heightMap;
}

float TerrainGenerator::at(int x, int y) const
{
    return m_heightMap[x][y];
}

void TerrainGenerator::setAt(int x, int y, float height)
{
    m_heightMap[x][y] = height;
}

void TerrainGenerator::resize(int width, int height)
{
    m_heightMap.resize(width);
    for (auto& row : m_heightMap) {
        row.resize(height, 0.0f);
    }
}

