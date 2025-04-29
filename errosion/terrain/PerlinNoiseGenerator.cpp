#include "PerlinNoiseGenerator.h"

#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

PerlinNoiseGenerator::PerlinNoiseGenerator()
    : m_octaves(4), m_persistence(0.5f)
{
    initPermutation(0);
}

PerlinNoiseGenerator::PerlinNoiseGenerator(unsigned int seed)
    : m_octaves(4), m_persistence(0.5f)
{
    initPermutation(seed);
}

PerlinNoiseGenerator::~PerlinNoiseGenerator() = default;

// Настройка параметров шума
void PerlinNoiseGenerator::setParameters(int octaves, float persistence, float scale)
{
    if (octaves < 1) octaves = 1;
    if (persistence <= 0.0f) persistence = 0.5f;
    // if (scale <= 0.0f) scale = 1.0f;

    m_octaves = octaves;
    m_persistence = persistence;
    m_scale = scale;
}

void PerlinNoiseGenerator::setSeed(unsigned int seed)
{
    initPermutation(seed);
}


// Реализация функции генерации шума
float PerlinNoiseGenerator::generateNoise(float x, float y)
{
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;


    for (int i = 0; i < m_octaves; i++)
    {
        total += perlin(x * frequency, y * frequency, 0.0f) * amplitude;

        maxValue += amplitude;

        amplitude *= m_persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

void PerlinNoiseGenerator::initPermutation(unsigned int seed)
{
    m_permutation.resize(256);
    std::iota(m_permutation.begin(), m_permutation.end(), 0);

    if (seed != 0)
    {
        std::default_random_engine engine(seed);
        std::shuffle(m_permutation.begin(), m_permutation.end(), engine);
    }
    else
    {
        std::random_device rd;
        std::mt19937 engine(rd());
        std::shuffle(m_permutation.begin(), m_permutation.end(), engine);
    }

    m_permutation.insert(m_permutation.end(), m_permutation.begin(), m_permutation.end());
}


float PerlinNoiseGenerator::fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoiseGenerator::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float PerlinNoiseGenerator::grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float PerlinNoiseGenerator::perlin(float x, float y, float z) const
{
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = static_cast<int>(floor(z)) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A = m_permutation[X] + Y;
    int AA = m_permutation[A] + Z;
    int AB = m_permutation[A + 1] + Z;
    int B = m_permutation[X + 1] + Y;
    int BA = m_permutation[B] + Z;
    int BB = m_permutation[B + 1] + Z;

    float res = lerp(w,
                     lerp(v,
                          lerp(u, grad(m_permutation[AA], x, y, z),
                               grad(m_permutation[BA], x - 1, y, z)),
                          lerp(u, grad(m_permutation[AB], x, y - 1, z),
                               grad(m_permutation[BB], x - 1, y - 1, z))),
                     lerp(v,
                          lerp(u, grad(m_permutation[AA + 1], x, y, z - 1),
                               grad(m_permutation[BA + 1], x - 1, y, z - 1)),
                          lerp(u, grad(m_permutation[AB + 1], x, y - 1, z - 1),
                               grad(m_permutation[BB + 1], x - 1, y - 1, z - 1)))
    );

    return res;
}
