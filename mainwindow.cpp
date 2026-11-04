#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboFunction->addItem("Прямокутні імпульси (Meander)");
    ui->comboFunction->addItem("Пилкоподібна хвиля (Sawtooth)");
    ui->comboFunction->addItem("Параболічна хвиля");

    ui->comboMethod->addItem("Центральних прямокутників");
    ui->comboMethod->addItem("Трапецій");
    ui->comboMethod->addItem("Сімпсона");

    setupCharts();

    connect(ui->btnBuild, &QPushButton::clicked, this, &MainWindow::onBuildGraphsClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupCharts() {
    waveformChartView = new QChartView();
    waveformChartView->setRenderHint(QPainter::Antialiasing);

    spectrumChartView = new QChartView();
    spectrumChartView->setRenderHint(QPainter::Antialiasing);

    ui->layoutWaveform->addWidget(waveformChartView);
    ui->layoutSpectrum->addWidget(spectrumChartView);
}

double MainWindow::periodicFunction(double x, int funcType, double T) {
    double x_norm = std::fmod(x, T);
    if (x_norm < 0) x_norm += T;

    switch (funcType) {
        case 0:
            if (x_norm < T / 2.0) return 1.0; else return -1.0;
        case 1:
            return (2.0 / T) * x_norm - 1.0;
        case 2:
            return std::pow(x_norm - T/2.0, 2);
        default: return 0.0;
    }
}

double MainWindow::calculateIntegral(MathFunc func, double a, double b, int n, int methodType) {
    double h = (b - a) / n;
    double sum = 0.0;

    switch (methodType) {
        case 0:
            for (int i = 0; i < n; ++i) sum += func(a + (i + 0.5) * h);
            return sum * h;
        case 1:
            sum = (func(a) + func(b)) / 2.0;
            for (int i = 1; i < n; ++i) sum += func(a + i * h);
            return sum * h;
        case 2:
            if (n % 2 != 0) n++;
            h = (b - a) / n;
            sum = func(a) + func(b);
            for (int i = 1; i < n; ++i) {
                double coef = (i % 2 == 0) ? 2.0 : 4.0;
                sum += coef * func(a + i * h);
            }
            return sum * (h / 3.0);
        default: return 0.0;
    }
}

void MainWindow::onBuildGraphsClicked() {
    double a = ui->spinA->value();
    double b = ui->spinB->value();

    if (a >= b) {
        QMessageBox::warning(this, "Помилка", "Початок інтервалу має бути меншим за кінець.");
        return;
    }

    int N = ui->spinHarmonics->value();
    int steps = ui->spinPoints->value();
    int funcIdx = ui->comboFunction->currentIndex();
    int methodIdx = ui->comboMethod->currentIndex();

    double T = b - a;
    double omega = 2 * M_PI / T;

    auto funcLambda = [&](double x) { return periodicFunction(x, funcIdx, T); };
    double a0 = (2.0 / T) * calculateIntegral(funcLambda, a, b, steps, methodIdx);

    QVector<double> an(N + 1), bn(N + 1), cn(N + 1);

    for (int n = 1; n <= N; ++n) {
        auto cosFunc = [&](double x) { return periodicFunction(x, funcIdx, T) * std::cos(n * omega * x); };
        an[n] = (2.0 / T) * calculateIntegral(cosFunc, a, b, steps, methodIdx);

        auto sinFunc = [&](double x) { return periodicFunction(x, funcIdx, T) * std::sin(n * omega * x); };
        bn[n] = (2.0 / T) * calculateIntegral(sinFunc, a, b, steps, methodIdx);

        cn[n] = std::sqrt(an[n]*an[n] + bn[n]*bn[n]);
    }
    cn[0] = std::abs(a0) / 2.0;

    QLineSeries *seriesOriginal = new QLineSeries();
    seriesOriginal->setName("f(x)");

    QLineSeries *seriesFourier = new QLineSeries();
    seriesFourier->setName(QString("Фур'є (N=%1)").arg(N));
    QPen penFourier(Qt::red);
    penFourier.setWidth(2);
    seriesFourier->setPen(penFourier);

    int plotPoints = 1000;
    double plotStep = (b - a) / plotPoints;

    for (int i = 0; i <= plotPoints; ++i) {
        double x = a + i * plotStep;
        seriesOriginal->append(x, periodicFunction(x, funcIdx, T));

        double y_fourier = a0 / 2.0;
        for (int n = 1; n <= N; ++n) {
            y_fourier += an[n] * std::cos(n * omega * x) + bn[n] * std::sin(n * omega * x);
        }
        seriesFourier->append(x, y_fourier);
    }

    QChart *chartWave = new QChart();
    chartWave->addSeries(seriesOriginal);
    chartWave->addSeries(seriesFourier);
    chartWave->createDefaultAxes();
    chartWave->setTitle("Результат");

    QChart *oldChartWave = waveformChartView->chart();
    waveformChartView->setChart(chartWave);
    if (oldChartWave) {
        oldChartWave->deleteLater();
    }

    QBarSet *set0 = new QBarSet("Амплітуда");
    QStringList categories;
    int limitVis = std::min(N, 20);

    for(int i = 0; i <= limitVis; ++i) {
        *set0 << cn[i];
        categories << QString::number(i);
    }

    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(set0);

    QChart *chartSpec = new QChart();
    chartSpec->addSeries(barSeries);
    chartSpec->setTitle("Спектр");
    chartSpec->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chartSpec->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chartSpec->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QChart *oldChartSpec = spectrumChartView->chart();
    spectrumChartView->setChart(chartSpec);
    if (oldChartSpec) {
        oldChartSpec->deleteLater();
    }
}