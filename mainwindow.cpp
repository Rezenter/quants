#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <complex>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Резонансное туннелирование");
    this->setFixedSize(this->size());
    chart = new QtCharts::QChart();
    axisX = new QtCharts::QValueAxis;
    axisX->setMin(0);
    axisX->setMinorTickCount(9);
    axisX->setTickCount(20);
    chart->legend()->hide();
    axisX->setTitleText("Energy[eV]");
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->setTitle("Transmission(E)");
    axisY = new QtCharts::QValueAxis;
    axisY->setMin(0);
    axisY->setMax(1.004);
    axisY->setMinorTickCount(1);
    axisY->setTickCount(10);
    axisY->setTitleText("transmission");
    chart->addAxis(axisY, Qt::AlignLeft);
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart, ui->chartWidget);
    chartView->resize(ui->chartWidget->size());
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->show();
    nValidator = new QRegExpValidator(QRegExp("[1-9]\\d{0,7}"), this);
    ui->nLine->setValidator(nValidator);
    uValidator = new QRegExpValidator(QRegExp("\\d{1,3}(\\.\\d{1,3})?"), this);
    validator = new QRegExpValidator(QRegExp("\\d{1,3}(\\.\\d{1,2})?"), this);
    ui->aLine->setValidator(validator);
    ui->uLine->setValidator(uValidator);
    ui->dLine->setValidator(validator);
    ui->aSlider->setTracking(false);
    ui->uSlider->setTracking(false);
    ui->dSlider->setTracking(false);
    ui->nSlider->setTracking(false);
    QObject::connect(ui->aSlider, &QSlider::valueChanged, this, &MainWindow::aResize);
    QObject::connect(ui->aSlider, &QSlider::sliderMoved, this, &MainWindow::aSet);
    QObject::connect(ui->uSlider, &QSlider::valueChanged, this, &MainWindow::uResize);
    QObject::connect(ui->uSlider, &QSlider::sliderMoved, this, &MainWindow::uSet);
    QObject::connect(ui->dSlider, &QSlider::valueChanged, this, &MainWindow::dResize);
    QObject::connect(ui->dSlider, &QSlider::sliderMoved, this, &MainWindow::dSet);
    QObject::connect(ui->nSlider, &QSlider::valueChanged, this, &MainWindow::nResize);
    QObject::connect(ui->nSlider, &QSlider::sliderMoved, this, &MainWindow::nSet);
    QObject::connect(ui->aLine, &QLineEdit::editingFinished, this, &MainWindow::aAdjust);
    QObject::connect(ui->aLine, &QLineEdit::textChanged, this, &MainWindow::aChange);
    QObject::connect(ui->uLine, &QLineEdit::editingFinished, this, &MainWindow::uAdjust);
    QObject::connect(ui->uLine, &QLineEdit::textChanged, this, &MainWindow::uChange);
    QObject::connect(ui->dLine, &QLineEdit::editingFinished, this, &MainWindow::dAdjust);
    QObject::connect(ui->dLine, &QLineEdit::textChanged, this, &MainWindow::dChange);
    QObject::connect(ui->nLine, &QLineEdit::editingFinished, this, &MainWindow::nAdjust);
    QObject::connect(ui->nLine, &QLineEdit::textChanged, this, &MainWindow::nChange);
    QObject::connect(ui->calculateButton, &QPushButton::pressed, this, &MainWindow::apply);


}

MainWindow::~MainWindow()
{
    delete chart;
    if(!series){
        delete series;
    }
    delete axisX;
    delete axisY;
    delete validator;
    delete uValidator;
    delete nValidator;
    delete ui;
}

void MainWindow::apply(){
    chart->removeAllSeries();
    series = new QtCharts::QLineSeries();
    a = ui->aLine->text().toDouble()*qPow(10, -9);
    u = ui->uLine->text().toDouble();
    d = ui->dLine->text().toDouble()*qPow(10, -9);
    int n = ui->nLine->text().toInt();
    for(int count = 1; count < n; count++){
        ui->progressBar->setValue(count*100/n);
        series->append(u*count/n, transmission(u*count/n));
    }
    ui->progressBar->setValue(100);
    chart->addSeries(series);

    axisX->setMax(u);
    //axisX->setLabelFormat("%i");


    series->attachAxis(axisX);

    //axisY->setLabelFormat("%d");

    series->attachAxis(axisY);

}

double MainWindow::transmission(double E){
    std::complex<double> k = qSqrt(2*mE*E*1.6021766208*qPow(10, -12)/qPow(dirak, 2));
    std::complex<double> kappa = qSqrt(2*mE*(u - E)*1.6021766208*qPow(10, -12)/qPow(dirak, 2));
    std::complex<double> sh = sinh(kappa*a);
    std::complex<double> ch = cosh(kappa*a);
    std::complex<double> translationValuesRev[4] = {
        ch, -(k/kappa)*sh,
        -(kappa/k)*sh, ch
    };
    QGenericMatrix<2, 2, std::complex<double>> translationMatrixRev(translationValuesRev);
    kappa = qSqrt(2*mE*E*1.6021766208*qPow(10, -12)/qPow(dirak, 2));
    std::complex<double> s = sin(kappa*d);
    std::complex<double> c = cos(kappa*d);
    std::complex<double> spreadValuesRev[4] = {
        c, -(k/kappa)*s,
        (kappa/k)*s, c
    };
    QGenericMatrix<2, 2, std::complex<double>> spreadMatrixRev(spreadValuesRev);
    QGenericMatrix<2, 2, std::complex<double>> matr = spreadMatrixRev*translationMatrixRev;
    matr = translationMatrixRev*matr;
    std::complex<double> i = -1;
    i = sqrt(i);
    std::complex<double> C = std::complex<double>(2)/(matr(0, 0) + matr(1, 1) + i*(matr(0, 1) - matr(1, 0)));
    return std::norm(C);
}

void MainWindow::aResize(int val){
    if((ui->aSlider->minimum() == val && ui->aSlider->minimum() != 0) || ui->aSlider->maximum() == val){
        ui->aSlider->setRange(val - 50, val + 50);
    }
    ui->aLine->setText(QString::number(double(val)/100, 'g', 5));
    ui->aLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::aSet(int val){
    ui->aLine->setText(QString::number(double(val)/100, 'g', 5));
    ui->aLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::uResize(int val){
    if((ui->uSlider->minimum() == val && ui->uSlider->minimum() != 0) || ui->uSlider->maximum() == val){
        ui->uSlider->setRange(val - 50, val + 50);
    }
    ui->uLine->setText(QString::number(double(val)/1000, 'g', 5));
    ui->uLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::uSet(int val){
    ui->uLine->setText(QString::number(double(val)/1000, 'g', 5));
    ui->uLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::dResize(int val){
    if((ui->dSlider->minimum() == val && ui->dSlider->minimum() != 0) || ui->dSlider->maximum() == val){
        ui->dSlider->setRange(val - 50, val + 50);
    }
    ui->dLine->setText(QString::number(double(val)/100, 'g', 5));
    ui->dLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::dSet(int val){
    ui->dLine->setText(QString::number(double(val)/100, 'g', 5));
    ui->dLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::nResize(int val){
    if((ui->nSlider->minimum() == val && ui->nSlider->minimum() > 100) || ui->nSlider->maximum() == val){
        ui->nSlider->setRange(val - 100, val + 100);
    }
    ui->nLine->setText(QString::number(val));
    ui->nLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::nSet(int val){
    ui->nLine->setText(QString::number(val));
    ui->nLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::aAdjust(){
    double x, y;
    y = std::modf(ui->aLine->text().toDouble()/0.5, &x);
    if(x>=1){
        ui->aSlider->setRange(x*50 - 50, x*50 + 50);
    }else{
        ui->aSlider->setRange(0, 50);
    }
    ui->aSlider->setValue((x + y)*50);
    ui->aLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::aChange(){
    ui->aLine->setStyleSheet("QLineEdit { background-color: yellow }");
    ui->progressBar->setValue(0);
}

void MainWindow::uAdjust(){
    double x, y;
    y = std::modf(ui->uLine->text().toDouble()/0.05, &x);
    if(x>=1){
        ui->uSlider->setRange(x*50 - 50, x*50 + 50);
    }else{
        ui->uSlider->setRange(0, 50);
    }
    ui->uSlider->setValue((x + y)*50);
    ui->uLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::uChange(){
    ui->uLine->setStyleSheet("QLineEdit { background-color: yellow }");
    ui->progressBar->setValue(0);
}

void MainWindow::dAdjust(){
    double x, y;
    y = std::modf(ui->dLine->text().toDouble()/0.5, &x);
    if(x>=1){
        ui->dSlider->setRange(x*50 - 50, x*50 + 50);
    }else{
        ui->dSlider->setRange(0, 50);
    }
    ui->dSlider->setValue((x + y)*50);
    ui->dLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::dChange(){
    ui->dLine->setStyleSheet("QLineEdit { background-color: yellow }");
    ui->progressBar->setValue(0);
}

void MainWindow::nAdjust(){
    double x, y;
    y = std::modf(ui->nLine->text().toDouble()/100, &x);
    if(x>=1){
        ui->nSlider->setRange(x*100 - 100, x*100+ 100);
    }else{
        ui->nSlider->setRange(0, 100);
    }
    ui->nSlider->setValue((x + y)*100);
    ui->nLine->setStyleSheet("QLineEdit { background-color: white }");
}

void MainWindow::nChange(){
    ui->nLine->setStyleSheet("QLineEdit { background-color: yellow }");
    ui->progressBar->setValue(0);
}
