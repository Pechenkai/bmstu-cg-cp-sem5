#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
      , m_renderWidget(nullptr)
{
    ui->setupUi(this);
    QWidget* container = ui->RenderWidget;

    m_renderWidget = new RenderWidget(this);

    m_renderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(m_renderWidget);
    container->setLayout(layout);

    QPalette palette = ui->color_lbl->palette();
    palette.setColor(QPalette::Window, Qt::white);
    ui->color_lbl->setAutoFillBackground(true);
    ui->color_lbl->setPalette(palette);

    connect(ui->generateBtn, &QPushButton::clicked, this, &MainWindow::onGenerateButtonClicked);
    connect(ui->errosionBtn, &QPushButton::clicked, this, &MainWindow::onErosionButtonClicked);
    connect(ui->forwardCam_btn, &QPushButton::clicked, this, &MainWindow::onForwardCamButtonClicked);
    connect(ui->backCam_btn, &QPushButton::clicked, this, &MainWindow::onBackCamButtonClicked);
    connect(ui->chColorbtn, &QPushButton::clicked, this, &MainWindow::onchButtonClicked);
    connect(ui->applyLight_btn, &QPushButton::clicked, this, &MainWindow::onappLightButtonClicked);
    connect(ui->retLandbtn, &QPushButton::clicked, this, &MainWindow::onretLandButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGenerateButtonClicked()
{
    int seed = ui->seedBox->value();
    int height = ui->lenBox->value();
    int width = ui->widBox->value();

    m_renderWidget->generateTerrain(seed, height, width);
}

void MainWindow::onErosionButtonClicked()
{
    int num_iter = ui->num_iterBox->value();
    int step_iter = ui->step_iterBox->value();
    float err_power = static_cast<float>(ui->errPowerBox->value()) / 10.f;
    float deposPow = static_cast<float>(ui->deposRateBox->value()) / 100.f;
    int num_drops = ui->numdropsBox->value();

    m_renderWidget->erode(num_iter, step_iter, err_power, deposPow, num_drops);
}

void MainWindow::onForwardCamButtonClicked()
{
    int dist = ui->camBox->value();

    m_renderWidget->moveCam(-dist);
}

void MainWindow::onBackCamButtonClicked()
{
    int dist = ui->camBox->value();

    m_renderWidget->moveCam(dist);
}

void MainWindow::onchButtonClicked()
{
    QPalette palette = ui->color_lbl->palette();
    QColor color = QColorDialog::getColor(Qt::white, this, "Выбор цвета");
    if (color.isValid())
    {
        palette.setColor(QPalette::Window, color);
    }
    else
    {
        color = Qt::white;
        palette.setColor(QPalette::Window, color);
    }

    ui->color_lbl->setAutoFillBackground(true);
    ui->color_lbl->setPalette(palette);

    m_renderWidget->setLightColor(color);
}

void MainWindow::onappLightButtonClicked()
{
    int intensity = ui->applyLight->value();

    m_renderWidget->setLightIntensity(intensity);
}

void MainWindow::onretLandButtonClicked()
{
    m_renderWidget->retHeightMap();
}
