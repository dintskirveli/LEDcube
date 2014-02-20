#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
QT_END_NAMESPACE

class MatrixWidget;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QSlider *createSlider(int min = 0, int range = 360, int singleStep = 1, int pageStep = 20, int tickInterval = 20);
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