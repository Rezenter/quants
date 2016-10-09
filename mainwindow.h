#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGenericMatrix>
#include <QMainWindow>
#include <QRegExpValidator>
#include <QtMath>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void apply();

private:
    Ui::MainWindow *ui;
    double a;
    double d;
    double u;
    double mE = 9.10938356 * qPow(10, -28);
    double dirak = 1.054571800 * qPow(10, -27);
    QRegExpValidator* nValidator;
    QRegExpValidator* validator;
    QRegExpValidator* uValidator;
    double transmission(double);
    QtCharts::QLineSeries *series;
    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;


private slots:
    void aSet(int);
    void aResize(int);
    void aAdjust();
    void aChange();
    void uSet(int);
    void uResize(int);
    void uAdjust();
    void uChange();
    void dSet(int);
    void dResize(int);
    void dAdjust();
    void dChange();
    void nSet(int);
    void nResize(int);
    void nAdjust();
    void nChange();

};

#endif // MAINWINDOW_H
