#include "RenderWidget.h"

#include <fstream>
#include <QApplication>
#include <QPainter>
#include <unistd.h>
#include <QKeyEvent>
#include <QDebug>
#include <QResizeEvent>
#include <time.h>

RenderWidget::RenderWidget(QWidget* parent)
    : QWidget(parent),
      m_renderer(nullptr), m_camera(static_cast<float>(width()) / height()),
      m_light(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0f))
{
    setFocusPolicy(Qt::StrongFocus);

    m_camera.updatePosition(0.3, 0, 0);

    const int terrainWidth = 30;
    const int terrainHeight = 30;

    m_noiseGenerator = new PerlinNoiseGenerator(15);
    m_noiseGenerator->setParameters(4, 0.3f, 1.f);

    m_terrainGenerator = new TerrainGenerator(terrainWidth, terrainHeight);
    m_terrainGenerator->setNoiseGenerator(m_noiseGenerator);
    m_terrainGenerator->generateHeightMap();

    m_erosion = new Erosion();

    m_oldheightMap = m_terrainGenerator->getHeightMap();

    m_mesh.generateMesh(*m_terrainGenerator);

    m_minHeight = m_terrainGenerator->getMinHeight();
    m_maxHeight = m_terrainGenerator->getMaxHeight();

    m_renderer = new Renderer(width(), height(), m_minHeight, m_maxHeight);

    connect(this, &RenderWidget::renderRequested, this, &RenderWidget::triggerRender);
}

RenderWidget::~RenderWidget()
{
    delete m_renderer;
    delete m_terrainGenerator;
    delete m_noiseGenerator;
    delete m_erosion;
}

void RenderWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);


    m_renderer->clearBuffers();
    m_renderer->render(m_mesh, m_camera, m_light);

    QPainter painter(this);
    painter.drawImage(0, 0, m_renderer->getFrameBuffer());
}

void RenderWidget::triggerRender()
{
    update();
}

void RenderWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_A)
    {
        m_camera.updatePosition(0, -0.1, 0);
        emit renderRequested();
    }
    else if (event->key() == Qt::Key_D)
    {
        m_camera.updatePosition(0, 0.1, 0);
        emit renderRequested();
    }
    else if (event->key() == Qt::Key_W)
    {
        m_camera.updatePosition(0.1, 0, 0);
        emit renderRequested();
    }
    else if (event->key() == Qt::Key_S)
    {
        m_camera.updatePosition(-0.1, 0, 0);
        emit renderRequested();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void RenderWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (m_renderer)
    {
        m_renderer->resize(width(), height());
    }

    m_camera.setAspectRatio(static_cast<float>(width()) / height());
}

void RenderWidget::generateTerrain(int seed, int height, int width)
{
    if (seed == 0)
        seed = time(NULL);

    m_noiseGenerator->setSeed(seed);

    m_terrainGenerator->setHeight(height);
    m_terrainGenerator->setWidth(width);
    m_terrainGenerator->resize(width, height);

    resize(width, height);

    m_terrainGenerator->setNoiseGenerator(m_noiseGenerator);
    m_terrainGenerator->generateHeightMap();

    m_oldheightMap = m_terrainGenerator->getHeightMap();

    m_mesh.generateMesh(*m_terrainGenerator);

    m_minHeight = m_terrainGenerator->getMinHeight();
    m_maxHeight = m_terrainGenerator->getMaxHeight();

    m_renderer->setMinMaxHeight(m_minHeight, m_maxHeight);

    emit renderRequested();
}

void RenderWidget::erode(int numIter, int num_steps, float errPow, float deposPow, int dropAmount)
{
    m_erosion->initMaps(*m_terrainGenerator);
    m_erosion->setErosionStrength(errPow);
    m_erosion->setDepositionRate(deposPow);

    for (int i = 0; i < numIter; i += num_steps)
    {
        for (int j = 0; j < num_steps && j < numIter; j++)
            m_erosion->applyDropletErosion(*m_terrainGenerator, dropAmount);

        m_mesh.generateMesh(*m_terrainGenerator);

        emit renderRequested();
        QApplication::processEvents();
    }
}

void RenderWidget::moveCam(int dist)
{
    m_camera.updatePosition(0, 0, dist);
    emit renderRequested();
}

void RenderWidget::setLightColor(QColor color)
{
    glm::vec3 col = glm::vec3(color.redF(), color.greenF(), color.blueF());

    m_light.setColor(col);
    emit renderRequested();
}

void RenderWidget::setLightIntensity(int intensity)
{
    m_light.setIntensity(intensity);
    emit renderRequested();
}

void RenderWidget::retHeightMap()
{
    m_terrainGenerator->setHeightMap(m_oldheightMap);
    m_mesh.generateMesh(*m_terrainGenerator);
    emit renderRequested();
}

void RenderWidget::resize(int width, int height)
{
    m_oldheightMap.resize(width);
    for (auto& row : m_oldheightMap)
    {
        row.resize(height, 0.0f);
    }
}

void RenderWidget::research()
{
    int num_exp = 100;
    std::vector<int> sizes = {30, 60, 90, 120, 150, 180, 210, 240, 270, 300};
    int seed = 0;

    std::ofstream resultsFile("../research/render_results.csv");

    resultsFile << "size,average_time_ms\n";

    for (auto size : sizes)
    {
        long long total_time = 0;

        for (int i = 0; i < num_exp; ++i)
        {
            seed = time(NULL);

            m_noiseGenerator->setSeed(seed);

            m_terrainGenerator->setHeight(size);
            m_terrainGenerator->setWidth(size);
            m_terrainGenerator->resize(size, size);

            m_terrainGenerator->setNoiseGenerator(m_noiseGenerator);
            m_terrainGenerator->generateHeightMap();

            m_mesh.generateMesh(*m_terrainGenerator);

            m_minHeight = m_terrainGenerator->getMinHeight();
            m_maxHeight = m_terrainGenerator->getMaxHeight();

            m_renderer->setMinMaxHeight(m_minHeight, m_maxHeight);

            m_renderer->clearBuffers();

            auto start = std::chrono::high_resolution_clock::now();
            m_renderer->render(m_mesh, m_camera, m_light);
            auto end = std::chrono::high_resolution_clock::now();

            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            total_time += elapsedTime;
        }

        double average_time = static_cast<double>(total_time) / num_exp;

        resultsFile << size << "," << average_time << "\n";
    }

    resultsFile.close();
}
