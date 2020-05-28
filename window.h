#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QComboBox>
#include <QtGui/QScreen>

#include "plot.h"

QT_BEGIN_NAMESPACE
class QGroupBox;
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);

private:
    Plot *plot;

    void setupKeyboardShortcuts();

    QGroupBox* createControlGroup();
    QPushButton* createToggleAnimationButton();
    QPushButton* createRestartAnimationButton();
    QSlider* createZoomSlider();
    QComboBox* createPlaybackSpeedBox();

    QTabWidget* createViewTabs();

    QGroupBox* createDescentGroup(Animation* animation,
        QFormLayout* layout);
    QGroupBox* createGradientDescentGroup();
    QGroupBox* createMomentumGroup();
    QGroupBox* createAdaGradGroup();
    QGroupBox* createRMSPropGroup();
    QGroupBox* createAdamGroup();

    QLayout* createLearningRateBox(GradientDescent* descent);
    QDoubleSpinBox* createDecayBox(double& val);

};

#endif

