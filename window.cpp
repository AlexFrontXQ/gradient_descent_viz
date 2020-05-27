#include <math.h>

#include <QtWidgets>

#include "window.h"


Window::Window(QWidget *parent)
 : QWidget(parent)
{
    setWindowTitle(QStringLiteral("Gradient Descent Visualization"));

    Surface *graph = new Surface();
    QWidget *container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    plot = new Plot(graph);

    vLayout->addWidget(createControlGroup());

    // widgets to tune gradient parameters
    vLayout->addWidget(createGradientDescentGroup());
    vLayout->addWidget(createMomentumGroup());
    vLayout->addWidget(createAdaGradGroup());
    vLayout->addWidget(createRMSPropGroup());
    vLayout->addWidget(createAdamGroup(), 1, Qt::AlignTop);

    setupKeyboardShortcuts();
}


void Window::setupKeyboardShortcuts(){
    QShortcut* left = new QShortcut(Qt::Key_Left, this);
    QObject::connect(left, &QShortcut::activated, [=](){plot->moveCamera(-1, 0);});
    QShortcut* right = new QShortcut(Qt::Key_Right, this);
    QObject::connect(right, &QShortcut::activated, [=](){plot->moveCamera(1, 0);});
    QShortcut* up = new QShortcut(Qt::Key_Up, this);
    QObject::connect(up, &QShortcut::activated, [=](){plot->moveCamera(0, 1);});
    QShortcut* down = new QShortcut(Qt::Key_Down, this);
    QObject::connect(down, &QShortcut::activated, [=](){plot->moveCamera(0, -1);});
    QShortcut* zoomin = new QShortcut(Qt::CTRL + Qt::Key_Equal, this);
    QObject::connect(zoomin, &QShortcut::activated, plot, &Plot::cameraZoomIn);
    QShortcut* zoomout = new QShortcut(Qt::CTRL + Qt::Key_Minus, this);
    QObject::connect(zoomout, &QShortcut::activated, plot, &Plot::cameraZoomOut);
}


QGroupBox *Window::createControlGroup(){
    QGroupBox *groupBox = new QGroupBox();
    QGridLayout *controlGrid= new QGridLayout;
    groupBox->setLayout(controlGrid);

    controlGrid->addWidget(createToggleAnimationButton(),0, 0);
    controlGrid->addWidget(createRestartAnimationButton(), 0, 1);
    controlGrid->addWidget(new QLabel(QStringLiteral("Zoom:")), 1, 0);
    controlGrid->addWidget(createZoomSlider(), 2, 0);
    controlGrid->addWidget(new QLabel(QStringLiteral("Playback speed:")), 1, 1);
    controlGrid->addWidget(createPlaybackSpeedBox(), 2, 1);

    return groupBox;
}


QPushButton *Window::createToggleAnimationButton(){
    // toggle animation button
    QPushButton *toggleAnimationButton = new QPushButton(this);

    toggleAnimationButton->setCheckable(true);
    toggleAnimationButton->setChecked(true);
    toggleAnimationButton->setText(QStringLiteral("Pause "));

    QObject::connect(toggleAnimationButton, &QPushButton::toggled,
        [=](bool is_checked){
            if (is_checked)
                toggleAnimationButton->setText(QStringLiteral("Pause "));
            else
                toggleAnimationButton->setText(QStringLiteral("Paused"));
        });

    QObject::connect(toggleAnimationButton, &QPushButton::clicked, plot,
                     &Plot::toggleAnimation);
    return toggleAnimationButton;
}


QPushButton *Window::createRestartAnimationButton(){
    // restart animation button
    QPushButton *restartAnimationButton = new QPushButton(this);
    restartAnimationButton->setText(QStringLiteral("Restart"));
    QObject::connect(restartAnimationButton, &QPushButton::clicked, plot,
                     &Plot::restartAnimation);
    return restartAnimationButton;
}

QSlider *Window::createZoomSlider(){
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(100, 1000);
    slider->setValue(120);
    plot->setCameraZoom(120);
    QObject::connect(slider, &QSlider::valueChanged, plot,
                     &Plot::setCameraZoom);
    return slider;
}


QComboBox *Window::createPlaybackSpeedBox(){
    QComboBox *box = new QComboBox(this);
    box->addItem("0.1x");
    box->addItem("0.2x");
    box->addItem("1x");
    box->addItem("5x");
    box->addItem("10x");
    box->setCurrentIndex(2);

    QObject::connect(box, SIGNAL(currentIndexChanged(int)),
                     plot, SLOT(setAnimationSpeed(int)));
    return box;
}


QGroupBox *Window::createDescentGroup(GradientDescent* descent,
                                      QFormLayout* layout){
    QGroupBox *groupBox = new QGroupBox(tr(descent->name));
    groupBox->setCheckable(true);
    groupBox->setChecked(true);

    QObject::connect(groupBox, &QGroupBox::clicked,
        [=](){
            descent->ball->setVisible(!descent->ball->isVisible());
        });

    groupBox->setStyleSheet(QString("QGroupBox::title {font: 10pt; border-radius: 5px; background: %1;}"
                                    ).arg(descent->ball_color.name()));

    groupBox->setLayout(layout);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setLabelAlignment(Qt::AlignLeft);
    return groupBox;
}


QGroupBox *Window::createGradientDescentGroup(){
    VanillaGradientDescent* descent = plot->gradient_descent.get();

    QFormLayout *form = new QFormLayout;
    form->addRow(new QLabel(QStringLiteral("Learning Rate:")),
                 createLearningRateBox(descent));

    return createDescentGroup(descent, form);
}


QGroupBox *Window::createMomentumGroup(){
    Momentum* descent = plot->momentum.get();

    QFormLayout *form = new QFormLayout;
    form->addRow(new QLabel(QStringLiteral("Learning Rate:")),
                 createLearningRateBox(descent));
    form->addRow(new QLabel(QStringLiteral("Decay rate:")),
                 createDecayBox(descent->decay_rate));

    return createDescentGroup(descent, form);
}


QGroupBox *Window::createAdaGradGroup(){
    AdaGrad* descent = plot->ada_grad.get();

    QFormLayout *form = new QFormLayout;
    form->addRow(new QLabel(QStringLiteral("Learning Rate:")),
                 createLearningRateBox(descent));

    return createDescentGroup(descent, form);
}


QGroupBox *Window::createRMSPropGroup(){
    RMSProp* descent = plot->rms_prop.get();

    QFormLayout *form = new QFormLayout;
    form->addRow(new QLabel(QStringLiteral("Learning Rate:")),
                 createLearningRateBox(descent));
    form->addRow(new QLabel(QStringLiteral("Decay rate:")),
                 createDecayBox(descent->decay_rate));

    return createDescentGroup(descent, form);
}


QGroupBox *Window::createAdamGroup(){
    Adam* descent = plot->adam.get();

    QFormLayout *form = new QFormLayout;
    form->addRow(new QLabel(QStringLiteral("Learning Rate:")),
                 createLearningRateBox(descent));
    form->addRow(new QLabel(QStringLiteral("Beta1:")),
                 createDecayBox(descent->beta1));
    form->addRow(new QLabel(QStringLiteral("Beta2:")),
                 createDecayBox(descent->beta2));

    return createDescentGroup(descent, form);
}


QLayout *Window::createLearningRateBox(GradientDescent* descent){
    // learning rate spin box
    QFormLayout *hbox = new QFormLayout;

    QSpinBox *learningRateBox = new QSpinBox(this);
    learningRateBox->setRange(-10, 10);
    learningRateBox->setValue(int(log(descent->learning_rate) / log(10)));
    QObject::connect(learningRateBox,
        QOverload<int>::of(&QSpinBox::valueChanged),
        [=](const int &newValue) {
            descent->learning_rate = pow(10, newValue);
        });

    hbox->addRow(new QLabel(QStringLiteral("1e")), learningRateBox);
    hbox->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    return hbox;
}


QDoubleSpinBox *Window::createDecayBox(double& val){
    QDoubleSpinBox *decayRateBox = new QDoubleSpinBox(this);
    decayRateBox->setDecimals(3);
    decayRateBox->setRange(0.0, 2.0);
    decayRateBox->setValue(val);
    decayRateBox->setSingleStep(0.1);
    QObject::connect(decayRateBox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        [&](const double &newValue ) {val = newValue;});
    return decayRateBox;
}
