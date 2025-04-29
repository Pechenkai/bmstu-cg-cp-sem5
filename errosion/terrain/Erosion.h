#ifndef ERROSION_H
#define ERROSION_H

#include <vector>
#include <algorithm>
#include <cmath>
#include "glm/glm.hpp"

#include "TerrainGenerator.h"

class Erosion
{
public:
    explicit Erosion(float erosionStrength = 0.5f);

    void initMaps(TerrainGenerator& terrain);

    void applyDropletErosion(TerrainGenerator& terrain,
                             int dropletCount = 300,
                             int maxSteps = 50,
                             float initialWater = 1.0f);

    void setErosionStrength(float erosionStrength);
    void setDepositionRate(float depositionRate);

private:
    int   m_width  = 0;
    int   m_height = 0;

    float m_erosionStrength;

    float m_inertia        = 0.05f;
    float m_evaporation    = 0.02f;
    float m_depositionRate = 0.05f;
    float m_erosionRate    = 0.1f;
    float m_capacity       = 1.0f;


private:
    void simulateDroplet(TerrainGenerator& terrain,
                         int startX, int startY,
                         int maxSteps,
                         float initialWater);

    bool inRange(int x, int y) const {
        return (x >= 0 && x < m_width && y >= 0 && y < m_height);
    }
};

#endif //ERROSION_H
