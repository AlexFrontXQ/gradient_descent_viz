#include <QtWidgets>

#include "window.h"

Window::Window(QWidget *parent)
 : QWidget(parent)
{
    setWindowTitle(QStringLiteral("Gradient Descent Visualization"));

    Q3DSurface *graph = new Q3DSurface();
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

    vLayout->addWidget(createToggleAnimationButton());
    vLayout->addWidget(createRestartAnimationButton());
    vLayout->addWidget(createGradientDescentGroup());
    vLayout->addWidget(createMomentumGroup(), 1, Qt::AlignTop);
//    vLayout->addWidget(createLearningRateBox(), 1, Qt::AlignTop);
}


QPushButton *Window::createToggleAnimationButton(){
    // toggle animation button
    QPushButton *toggleAnimationButton = new QPushButton(this);

    toggleAnimationButton->setCheckable(true);
    toggleAnimationButton->setChecked(true);

    toggleAnimationButton->setText(QStringLiteral("Pause"));
    QObject::connect(toggleAnimationButton, &QPushButton::clicked, plot,
                     &Plot::toggleAnimation);
    QObject::connect(toggleAnimationButton, &QPushButton::clicked,
        [=](){
            if (toggleAnimationButton->text() == QStringLiteral("Play"))
                toggleAnimationButton->setText(QStringLiteral("Pause"));
            else
                toggleAnimationButton->setText(QStringLiteral("Play"));
        });
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


QGroupBox *Window::createGradientDescentGroup(){
    QGroupBox *groupBox = new QGroupBox(tr("&Gradient Descent"));
    groupBox->setCheckable(true);
    groupBox->setChecked(true);

    GradientDescent* descent = plot->gradient_descent.get();

    QObject::connect(groupBox, &QGroupBox::clicked,
        [=](){
            descent->ball->setVisible(!descent->ball->isVisible());
        });

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(new QLabel(QStringLiteral("Learning Rate:")));
    vbox->addWidget(createLearningRateBox(descent));
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}

QGroupBox *Window::createMomentumGroup(){
    QGroupBox *groupBox = new QGroupBox(tr("&Momemtum"));
    groupBox->setCheckable(true);
    groupBox->setChecked(true);

    Momentum* descent = plot->momemtum.get();

    QObject::connect(groupBox, &QGroupBox::clicked,
        [=](){
            descent->ball->setVisible(!descent->ball->isVisible());
        });

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(new QLabel(QStringLiteral("Learning Rate:")));
    vbox->addWidget(createLearningRateBox(descent));
    vbox->addWidget(new QLabel(QStringLiteral("Momentum:")));
    vbox->addWidget(createMomentumBox(descent));
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}


QDoubleSpinBox *Window::createLearningRateBox(GradientDescent* descent){
    // learning rate spin box
    QDoubleSpinBox *learningRateBox = new QDoubleSpinBox(this);
    learningRateBox->setDecimals(4);
    learningRateBox->setRange(0.0001, 1.0);
    learningRateBox->setValue(descent->learning_rate);
    learningRateBox->setSingleStep(0.001);
    QObject::connect(learningRateBox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        [=]( const double &newValue ) {
            descent->learning_rate = newValue;
        });
    return learningRateBox;
}


QDoubleSpinBox *Window::createMomentumBox(Momentum* descent){
    // learning rate spin box
    QDoubleSpinBox *learningRateBox = new QDoubleSpinBox(this);
    learningRateBox->setDecimals(1);
    learningRateBox->setRange(0.1, 2.0);
    learningRateBox->setValue(descent->momentum);
    learningRateBox->setSingleStep(0.1);
    QObject::connect(learningRateBox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        [=]( const double &newValue ) {
            descent->momentum = newValue;
        });
    return learningRateBox;
}


