#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H


class NoiseGenerator
{
public:
    virtual ~NoiseGenerator() = default;

    virtual float generateNoise(float x, float y) = 0;
};


#endif //NOISEGENERATOR_H
