#include "Erosion.h"

#include "Erosion.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

Erosion::Erosion(float erosionStrength)
    : m_erosionStrength(erosionStrength)
{
}

void Erosion::initMaps(TerrainGenerator& terrain)
{
    m_width  = terrain.getWidth();
    m_height = terrain.getHeight();
}

void Erosion::setErosionStrength(float erosionStrength)
{
    m_erosionStrength = erosionStrength;
}

void Erosion::setDepositionRate(float depositionRate)
{
    m_depositionRate = depositionRate;
}




void Erosion::applyDropletErosion(TerrainGenerator& terrain,
                                  int dropletCount,
                                  int maxSteps,
                                  float initialWater)
{

    for (int i = 0; i < dropletCount; ++i)
    {
        int x0 = rand() % m_width;
        int y0 = rand() % m_height;

        simulateDroplet(terrain, x0, y0, maxSteps, initialWater);
    }
}


void Erosion::simulateDroplet(TerrainGenerator& terrain,
                              int startX, int startY,
                              int maxSteps,
                              float initialWater)
{
    int x = startX;
    int y = startY;

    float water    = initialWater;
    float sediment = 0.0f;

    for (int step = 0; step < maxSteps; ++step)
    {
        float currentHeight = terrain.at(x, y);

        float lowestHeight = currentHeight;
        int nx = x;
        int ny = y;

        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx_ = -1; dx_ <= 1; ++dx_)
            {
                if (dx_ == 0 && dy == 0) continue;
                int xx = x + dx_;
                int yy = y + dy;
                if (!inRange(xx, yy)) continue;

                float neighH = terrain.at(xx, yy);
                if (neighH < lowestHeight) {
                    lowestHeight = neighH;
                    nx = xx;
                    ny = yy;
                }
            }
        }

        if (nx == x && ny == y)
        {

            float deposit = sediment * m_depositionRate;
            terrain.setAt(x, y, currentHeight + deposit);
            sediment -= deposit;

            break;
        }

        float slope = (currentHeight - lowestHeight);

        float erodeAmount = m_erosionRate * m_erosionStrength * slope;

        if (currentHeight > 0)
            erodeAmount = std::min(erodeAmount, currentHeight);

        if (erodeAmount > 0)
        {
            terrain.setAt(x, y, currentHeight - erodeAmount);
            sediment += erodeAmount;
        }

        float capacity = m_capacity * water;
        if (sediment > capacity)
        {
            float deposit = (sediment - capacity) * m_depositionRate;
            float newHeight = terrain.at(x,y) + deposit;
            terrain.setAt(x, y, newHeight);
            sediment -= deposit;
        }

        x = nx;
        y = ny;

        water -= (water * m_evaporation);
        if (water <= 0.00001f)
        {
            if (sediment > 0.0f)
            {
                float oldH = terrain.at(x,y);
                terrain.setAt(x, y, oldH + sediment);
                sediment = 0.0f;
            }
            break;
        }
    }
}
