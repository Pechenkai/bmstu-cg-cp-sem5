#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H


#include <vector>
#include <iostream>
#include "NoiseGenerator.h"

class TerrainGenerator
{
public:
    TerrainGenerator(int width, int height);
    ~TerrainGenerator();

    void generateHeightMap();

    void setNoiseGenerator(NoiseGenerator* generator);
    void setScale(float scale);

    [[nodiscard]] const std::vector<std::vector<float>> &getHeightMap() const;
    void setHeightMap(const std::vector<std::vector<float>> &heightMap);

    int getWidth() const;
    int getHeight() const;

    void setHeight(int height);
    void setWidth(int width);

    float getMinHeight() const;
    float getMaxHeight() const;

    float at(int x, int y) const;
    void setAt(int x, int y, float height);
    void resize(int width, int height);

    void calculateScale();

private:
    int m_width;
    int m_height;
    float m_scale;

    std::vector<std::vector<float>> m_heightMap;

    NoiseGenerator* m_noiseGenerator;
};

#endif //TERRAINGENERATOR_H
