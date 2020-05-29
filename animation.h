#ifndef ANIMATION_H
#define ANIMATION_H

#include <memory>

#include <QtCore/QTimer>
#include <QtDataVisualization/QCustom3DItem>
#include <QtDataVisualization/Q3DSurface>

#include "gradient_descent.h"

using namespace  QtDataVisualization;

const auto f = GradientDescent::f;
const float kBallYOffset = 10.f;
const float stepX = 4. / 49;
const float stepZ = 4. / 49;
const int kInterval = 1000; // seconds in between steps
const QColor gradient_color = Qt::cyan;
const QColor momentum_color = Qt::magenta;
const float simpleAnimationArrowScale = 0.2;

class Animation
{
public:
    Animation(Q3DSurface* _graph, QTimer* _timer)
        : m_graph(_graph),
          timer(_timer) {}
    virtual ~Animation(){}

    QString name;
    QColor ball_color;

    void triggerDetailedAnimation(int animation_speedup);
    virtual void triggerSimpleAnimation(int animation_speedup,
        bool show_gradient, bool show_adjusted_gradient,
        bool show_momentum, bool show_gradient_squared);
    std::unique_ptr<GradientDescent> descent;
    void cleanupAll();
    void cleanupGradient();
    void cleanupAdjustedGradient();
    void cleanupMomentum();
    void cleanupGradientSquared();
    void setVisible(bool visible);
    void restartAnimation();

protected:
    int num_states;
    int state = 0;
    bool in_initial_state = true;
    bool has_momentum = false;
    bool has_gradient_squared = false;
    bool m_visible = true;
    bool detailed_animation_prepared = false;

    Q3DSurface* m_graph;
    QTimer* timer;

    // visual elements (applicable to all descents)
    std::unique_ptr<Ball> ball;
    std::unique_ptr<Ball> temporary_ball = nullptr;
    std::unique_ptr<Arrow> arrowX = nullptr;
    std::unique_ptr<Arrow> arrowZ = nullptr;
    std::unique_ptr<Arrow> adjustedArrowX = nullptr;
    std::unique_ptr<Arrow> adjustedArrowZ = nullptr;
    std::unique_ptr<Arrow> total_arrow = nullptr;
    // visual elements (applicable to some descents)
    std::unique_ptr<Arrow> momentumArrowX = nullptr;
    std::unique_ptr<Arrow> momentumArrowZ = nullptr;
    std::unique_ptr<Square> squareX = nullptr;
    std::unique_ptr<Square> squareZ = nullptr;

    virtual void animateStep() = 0;
    virtual int interval(){return kInterval;}
    virtual void prepareDetailedAnimation();

    void animateGradient();
    void animateAdjustedGradient();
    void animateMomentum();
    void animateGradientSquared();
    virtual Point momentum(){return Point();};
    virtual Point gradSumOfSquared(){return Point();};
};


class GradientDescentAnimation : public Animation
{
public:
    GradientDescentAnimation(Q3DSurface* _graph, QTimer* _timer)
        : Animation(_graph, _timer)
    {
        name = "Gradient Descent";
        num_states = 4;
        ball_color = gradient_color;
        ball = std::unique_ptr<Ball>(new Ball(m_graph, ball_color, f));
        descent = std::unique_ptr<GradientDescent>(new VanillaGradientDescent);
    };


    void animateStep();

protected:

};

class MomentumAnimation : public Animation
{
public:
    MomentumAnimation(Q3DSurface* _graph, QTimer* _timer)
        : Animation(_graph, _timer)
    {
        name = "Momentum";
        num_states = 6;
        ball_color = Qt::magenta;
        ball = std::unique_ptr<Ball>(new Ball(m_graph, ball_color, f));
        descent = std::unique_ptr<GradientDescent>(new Momentum);
        has_momentum = true;
    };

    void prepareDetailedAnimation();
    void animateStep();

protected:
    Point momentum(){return Point(-descent->delta().x / descent->learning_rate,
                                  -descent->delta().z / descent->learning_rate);}
};


class AdaGradAnimation : public Animation
{
public:
    AdaGradAnimation(Q3DSurface* _graph, QTimer* _timer)
        : Animation(_graph, _timer)
    {
        name = "Adagrad";
        num_states = 6;
        ball_color = Qt::white;
        ball = std::unique_ptr<Ball>(new Ball(m_graph, ball_color, f));
        descent = std::unique_ptr<GradientDescent>(new AdaGrad);
        has_gradient_squared = true;
    };

    void prepareDetailedAnimation();
    void animateStep();

protected:
    // scale up the arrow, otherwise you can't see because adagrad moves so slow
    const float arrowScale = 1;

    Point gradSumOfSquared() {
        return dynamic_cast<AdaGrad*> (descent.get())->gradSumOfSquared();}
};


class RMSPropAnimation : public Animation
{
public:
    RMSPropAnimation(Q3DSurface* _graph, QTimer* _timer)
        : Animation(_graph, _timer)
    {
        name = "RMSprop";
        num_states = 7;
        ball_color = Qt::green;
        ball = std::unique_ptr<Ball>(new Ball(m_graph, ball_color, f));
        descent = std::unique_ptr<GradientDescent>(new RMSProp);
        has_gradient_squared = true;
    };

    void prepareDetailedAnimation();
    void animateStep();

protected:
    const float arrowScale = 1;

    Point gradSumOfSquared(){
        return dynamic_cast<RMSProp*> (descent.get())->decayedGradSumOfSquared();}
};


class AdamAnimation : public Animation
{
public:
    AdamAnimation(Q3DSurface* _graph, QTimer* _timer)
        : Animation(_graph, _timer)
    {
        name = "Adam";
        num_states = 9;
        ball_color = Qt::blue;
        ball = std::unique_ptr<Ball>(new Ball(m_graph, ball_color, f));
        descent = std::unique_ptr<GradientDescent>(new Adam);
        has_momentum = true;
        has_gradient_squared = true;
    };

    void prepareDetailedAnimation();
    void animateStep();

protected:
    // scale up the arrow, otherwise you can't see because adagrad moves so slow
    const float arrowScale = 1;
    int interval() {return 2000;}

    Point momentum() {
        return dynamic_cast<Adam*> (descent.get())->decayedGradSum();} 
    Point gradSumOfSquared(){
        return dynamic_cast<Adam*> (descent.get())->decayedGradSumOfSquared();}

};

#endif // ANIMATION_H
