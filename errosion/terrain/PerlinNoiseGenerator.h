#ifndef PERLINNOISEGENERATOR_H
#define PERLINNOISEGENERATOR_H

#include "NoiseGenerator.h"
#include <vector>

class PerlinNoiseGenerator : public NoiseGenerator {
public:
    PerlinNoiseGenerator();
    explicit PerlinNoiseGenerator(unsigned int seed);

    ~PerlinNoiseGenerator() override;

    float generateNoise(float x, float y) override;

    void setParameters(int octaves, float persistence, float scale);

    void setSeed(unsigned int seed);

private:
    int m_octaves;
    float m_persistence;
    float m_scale;

    std::vector<int> m_permutation;

    float perlin(float x, float y, float z) const;
    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y, float z);

    void initPermutation(unsigned int seed);
};

#endif //PERLINNOISEGENERATOR_H
