#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
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
    Window();

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

    QSpinBox* xSize;
    QSpinBox* ySize;
    QSpinBox* zSize;
};

#endif