#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include "./widget/RenderWidget.h"
#include "./terrain/Erosion.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateButtonClicked();
    void onErosionButtonClicked();
    void onForwardCamButtonClicked();
    void onBackCamButtonClicked();
    void onchButtonClicked();
    void onappLightButtonClicked();
    void onretLandButtonClicked();

private:
    Ui::MainWindow* ui;
    RenderWidget* m_renderWidget;
};
#endif // MAINWINDOW_H
