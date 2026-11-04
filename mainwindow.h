#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBuildGraphsClicked();

private:
    Ui::MainWindow *ui;
    QChartView *waveformChartView;
    QChartView *spectrumChartView;

    using MathFunc = std::function<double(double)>;
    double periodicFunction(double x, int funcType, double T);
    double calculateIntegral(MathFunc func, double a, double b, int n, int methodType);

    void setupCharts();
};

#endif