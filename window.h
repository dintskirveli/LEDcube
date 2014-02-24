#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "libfreenect.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
class QCheckBox;
class QLabel;
QT_END_NAMESPACE

class MatrixWidget;

//! The main window of the application
/*!
    Is the container for the LEDMatrix widget and the various sliders, spinboxes, etc...
    
*/
class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(freenect_device *dev = 0, freenect_context *ctx = 0);

public slots:
	void setSpacingSliderEnabled(bool enabled);
	void setCubicDimensions(bool cubic);
	void maybeSetAllDimensions(int value);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QSlider *createSlider(int min = 0, int max = 360, int singleStep = 1, int pageStep = 20, int tickInterval = 20);
    QSpinBox* createSpinBox();
    MatrixWidget *matrixWidget;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    QSlider* zoomSlider;
    QSlider* spacingSlider;
    QSpinBox* xSize;
    QSpinBox* ySize;
    QSpinBox* zSize;
    QCheckBox* drawOff;
    QCheckBox* isCube;
    int drawMode;
    
    QLabel *xSliderLabel;
    QLabel *ySliderLabel;
    QLabel *zSliderLabel;
    QLabel *zoomLabel;
    QLabel *xSizeLabel;
    QLabel *ySizeLabel;
    QLabel *zSizeLabel;
    QLabel *spaceLabel;
};

#endif