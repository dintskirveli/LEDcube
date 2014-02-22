#include <QtGui>

#include "matrixwidget.h"
#include "window.h"

Window::Window()
{
    matrixWidget = new MatrixWidget;
    matrixWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    QSettings *settings = new QSettings("groupname", "LEDcube");

    drawMode = settings->value("drawMode", MatrixWidget::MODE_POINTS).toInt();

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();
    zoomSlider = createSlider(-360);

    xSize = createSpinBox();
    ySize = createSpinBox();
    zSize = createSpinBox();

    xSliderLabel = new QLabel(tr("X-axis"));
    xSliderLabel->setBuddy(xSlider);
    xSliderLabel->setAlignment(Qt::AlignCenter);
    
    ySliderLabel = new QLabel(tr("Y-axis"));
    ySliderLabel->setBuddy(ySlider);
    ySliderLabel->setAlignment(Qt::AlignCenter);
    
    zSliderLabel = new QLabel(tr("Z-axis"));
    zSliderLabel->setBuddy(zSlider);
    zSliderLabel->setAlignment(Qt::AlignCenter);
    
    zoomLabel = new QLabel(tr("Zoom"));
    zoomLabel->setBuddy(zoomSlider);
    zoomLabel->setAlignment(Qt::AlignCenter);

    xSizeLabel = new QLabel(tr("x-Size"));
    xSizeLabel->setBuddy(xSize);
    xSizeLabel ->setAlignment(Qt::AlignCenter);
    
    ySizeLabel = new QLabel(tr("y-Size"));
    ySizeLabel->setBuddy(ySize);
    ySizeLabel ->setAlignment(Qt::AlignCenter);
    
    zSizeLabel = new QLabel(tr("z-Size"));
    zSizeLabel->setBuddy(zSize);
    zSizeLabel ->setAlignment(Qt::AlignCenter);
    
    spacingSlider = new QSlider(Qt::Horizontal);
    spacingSlider->setRange(0, 20);
    spacingSlider->setSingleStep(1);
    spacingSlider->setPageStep(1); 
    spacingSlider->setTickInterval(1);

    spaceLabel = new QLabel(tr("Cube Size"));
    spaceLabel->setBuddy(spacingSlider);
    spaceLabel->setAlignment(Qt::AlignCenter);


    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Cubes"));
    comboBox->addItem(tr("Points"));
    if (drawMode == MatrixWidget::MODE_CUBES) {
        comboBox->setCurrentIndex (0);
        spacingSlider->setEnabled(true);
    } else if (drawMode == MatrixWidget::MODE_POINTS) {
        comboBox->setCurrentIndex (1);
        spacingSlider->setEnabled(false);
    }

    drawOff = new QCheckBox("draw \"Off\" LEDs?");
    isCube = new QCheckBox("Keep dimensions cubic");

    connect(xSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXRotation(int)));
    connect(matrixWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYRotation(int)));
    connect(matrixWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZRotation(int)));
    connect(matrixWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));

    connect(zoomSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZoom(int)));
    connect(spacingSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setSpacing(int)));
    connect(comboBox, SIGNAL(activated(int)), matrixWidget, SLOT(setMode(int)));
    connect(matrixWidget, SIGNAL(setSpacingSliderEnabled(bool)), this, SLOT(setSpacingSliderEnabled(bool)));

    connect(drawOff, SIGNAL(toggled(bool)), matrixWidget, SLOT(toggleDrawOff(bool)));
    connect(isCube, SIGNAL(toggled(bool)), this, SLOT(setCubicDimensions(bool)));        

    connect(xSize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXSize(int)));
    connect(xSize, SIGNAL(valueChanged(int)), this, SLOT(maybeSetAllDimensions(int)));
    connect(ySize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYSize(int)));   
    connect(zSize, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZSize(int)));     

    QVBoxLayout *settingsLayout = new QVBoxLayout;

    QHBoxLayout *sliders = new QHBoxLayout;
    QHBoxLayout *sliderlabels = new QHBoxLayout;

    QHBoxLayout *spinBoxes = new QHBoxLayout;
    QHBoxLayout *spinBoxesLabels = new QHBoxLayout;

    sliderlabels->addWidget(xSliderLabel);
    sliderlabels->addWidget(ySliderLabel);
    sliderlabels->addWidget(zSliderLabel);
    sliderlabels->addWidget(zoomLabel);

    spinBoxesLabels->addWidget(xSizeLabel);
    spinBoxesLabels->addWidget(ySizeLabel);
    spinBoxesLabels->addWidget(zSizeLabel);

    sliders->addWidget(xSlider);
    sliders->addWidget(ySlider);
    sliders->addWidget(zSlider);
    sliders->addWidget(zoomSlider);
    
    spinBoxes->addWidget(xSize);
    spinBoxes->addWidget(ySize);
    spinBoxes->addWidget(zSize);

    settingsLayout->addLayout(sliderlabels);
    settingsLayout->addLayout(sliders);
    settingsLayout->addLayout(spinBoxes);
    settingsLayout->addLayout(spinBoxesLabels);
    settingsLayout->addWidget(spacingSlider);
    settingsLayout->addWidget(spaceLabel);
    settingsLayout->addWidget(comboBox);
    settingsLayout->addWidget(drawOff);
    settingsLayout->addWidget(isCube);
    
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QWidget *settingsWidget = new QWidget();
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setMaximumWidth(200);
    mainLayout->addWidget(matrixWidget);
    mainLayout->addWidget(settingsWidget);

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
    spacingSlider->setValue(settings->value("spacing", .5f).toFloat()*10);

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
    spin->setRange(0, 100);
    return spin;
}

void Window::setSpacingSliderEnabled(bool enabled) {
    spacingSlider->setEnabled(enabled);
}

void Window::setCubicDimensions(bool cubic) {
    if (cubic) {
        int val = xSize->value();
        ySize->setEnabled(false);
        zSize->setEnabled(false);
        ySize->setValue(val);
        zSize->setValue(val);
    } else {
        ySize->setEnabled(true);
        zSize->setEnabled(true);
    }
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::maybeSetAllDimensions(int val) {
    if (isCube->isChecked()) {
        ySize->setValue(val);
        zSize->setValue(val);
    }
}
