#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QTimer>
#include "../rendering/Renderer.h"
#include "../geometry/Mesh.h"
#include "../rendering/Camera.h"
#include "../rendering/Light.h"
#include "../terrain/TerrainGenerator.h"
#include "../terrain/PerlinNoiseGenerator.h"
#include "../terrain/Erosion.h"


class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget* parent = nullptr);
    void generateTerrain(int seed, int height, int width);
    void erode(int numIter, int num_steps, float errPow, float deposPow, int dropAmount);
    void moveCam(int dist);
    void setLightColor(QColor color);
    void setLightIntensity(int intensity);
    void retHeightMap();
    void resize(int width, int height);
    ~RenderWidget();

signals:
    void renderRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void research();


private slots:
    void triggerRender();

private:
    Renderer* m_renderer;
    Mesh m_mesh;
    PerlinNoiseGenerator* m_noiseGenerator;
    TerrainGenerator* m_terrainGenerator;
    Erosion* m_erosion;

    Camera m_camera;
    Light m_light;

    std::vector<std::vector<float>> m_oldheightMap;

    float m_minHeight;
    float m_maxHeight;

    bool needRender = true;
};


#endif //RENDERWIDGET_H
