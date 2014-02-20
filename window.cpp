#include <QtGui>

#include "matrixwidget.h"
#include "window.h"

Window::Window()
{
    matrixWidget = new MatrixWidget;
    QSettings *settings = new QSettings("groupname", "LEDcube");

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();
    zoomSlider = createSlider(-360); //make min = 360

    xSize = createSpinBox();
    ySize = createSpinBox();
    zSize = createSpinBox();

    connect(xSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXRotation(int)));
    connect(matrixWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYRotation(int)));
    connect(matrixWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZRotation(int)));
    connect(matrixWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));

    connect(zoomSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZoom(int)));

    connect(xSize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXSize(int)));
    connect(ySize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYSize(int)));   
    connect(zSize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZSize(int)));     

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(matrixWidget);
    
    QVBoxLayout *xLayout = new QVBoxLayout;

    xLayout->addWidget(xSlider);
    xLayout->addWidget(xSize);

    QVBoxLayout *yLayout = new QVBoxLayout;

    yLayout->addWidget(ySlider);
    yLayout->addWidget(ySize);

    QVBoxLayout *zLayout = new QVBoxLayout;

    zLayout->addWidget(zSlider);
    zLayout->addWidget(zSize);

    mainLayout->addLayout(xLayout);
    mainLayout->addLayout(yLayout);
    mainLayout->addLayout(zLayout);

    mainLayout->addWidget(zoomSlider);

    QWidget *central = new QWidget();
    central->setLayout(mainLayout);
    setCentralWidget(central);

    xSlider->setValue(0);
    ySlider->setValue(0);
    zSlider->setValue(0);

    xSize->setValue(settings->value("xSize", 20).toInt());
    ySize->setValue(settings->value("ySize", 20).toInt());
    zSize->setValue(settings->value("zSize", 20).toInt());

    zoomSlider->setValue(0);
    setWindowTitle(tr("LEDcube"));
}

QSlider *Window::createSlider(int min, int max, int singleStep, int pageStep, int tickInterval)
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(min, max);
    slider->setSingleStep(singleStep);
    slider->setPageStep(pageStep); 
    slider->setTickInterval(tickInterval);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSpinBox *Window::createSpinBox()
{
    QSpinBox *spin = new QSpinBox();
    spin->setRange(0, 50);
    spin->setMaximumWidth(50);
    return spin;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
