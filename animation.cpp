#include "animation.h"

void AnimationHelper::setBallPositionOnSurface(Ball* ball, Point p){
    const float cutoff = 15;
    float y = f(p.x, p.z);
    // hack: if the graph has a hole that's too deep, we can't see the ball
    // hardcode to lift the ball up
    if (f(p.x + stepX, p.z) - y > cutoff ||
        f(p.x, p.z + stepZ) - y > cutoff){
        y = std::max(f(p.x + stepX, p.z),
                f(p.x, p.z + stepZ) - y) - cutoff - 10;
    }
    else{
        // to make the ball look like it's above the surface
        y += kBallYOffset;
    }
    ball->setPosition(QVector3D(p.x, y, p.z));
}


void Animation::triggerAnimation(){
    animateStep();
    timer->setInterval(kInterval);
    state = (state + 1) % num_states;
}


void Animation::prepareDetailedAnimation(){
//    QColor color = descent->ball_color;
    QColor color = Qt::magenta;
    color.setAlpha(100);
    arrowX = std::unique_ptr<Arrow>(new Arrow(QVector3D(-1, 0, 0)));
    arrowX->setMagnitude(0);
    arrowX->setLabel("gradient in x");
    arrowX->addToGraph(m_graph);
    arrowX->setVisible(false);
    arrowZ = std::unique_ptr<Arrow>(new Arrow(QVector3D(0, 0, -1)));
    arrowZ->setMagnitude(0);
    arrowZ->setLabel("gradient in z");
    arrowZ->addToGraph(m_graph);
    arrowZ->setVisible(false);
    total_arrow = std::unique_ptr<Arrow>(new Arrow);
    total_arrow->setLabel("total gradient");
    total_arrow->addToGraph(m_graph);
    total_arrow->setVisible(false);
    temporary_ball = std::unique_ptr<Ball>(new Ball(color));
    temporary_ball->addToGraph(m_graph);
}


void GradientDescentAnimation::animateStep(){
    switch(state){
    case 0: // just show the ball
    {
        arrowX->setVisible(false);
        arrowZ->setVisible(false);
        total_arrow->setVisible(false);
        Point p = descent->position();
        AnimationHelper::setBallPositionOnSurface(descent->ball.get(), p);
        break;
    }
    case 1: // show the x and z direction gradients
    { 
        Point grad(descent->gradX(), descent->gradZ());
        arrowX->setMagnitude(grad.x);
        arrowZ->setMagnitude(grad.z);
        for (Arrow* arrow : {arrowX.get(), arrowZ.get()})
        {
            arrow->setPosition(descent->ball->position());
            arrow->setVisible(true);
        }
        break;
    }
    case 2: // show the composite of gradients
    {
        descent->takeGradientStep();
        Point delta = descent->delta();
        total_arrow->setVector(QVector3D(delta.x, 0, delta.z) /
                               descent->learning_rate);
        total_arrow->setPosition(descent->ball->position());
        total_arrow->setVisible(true);
        break;
    }
    case 3: // draw an imaginary ball of the future position
    {
        Point p = descent->takeGradientStep();
        AnimationHelper::setBallPositionOnSurface(temporary_ball.get(), p);
        break;
    }
    }
}


void MomentumAnimation::prepareDetailedAnimation(){
    Animation::prepareDetailedAnimation();
    momentumArrowX = std::unique_ptr<Arrow>(new Arrow(QVector3D(-1, 0, 0)));
    momentumArrowX->setLabel("momentum x");
    momentumArrowX->setMagnitude(0);
    momentumArrowZ = std::unique_ptr<Arrow>(new Arrow(QVector3D(0, 0, -1)));
    momentumArrowZ->setMagnitude(0);
    for (Arrow* arrow : {momentumArrowX.get(), momentumArrowZ.get()}){
        arrow->setColor(descent->ball_color);
        arrow->setMagnitude(0.1);
        arrow->addToGraph(m_graph);
    }
}


void MomentumAnimation::animateStep(){
    switch(state){
    case 0: // the ball and momentum arrows
    {
        Point p = descent->position();
        AnimationHelper::setBallPositionOnSurface(descent->ball.get(), p);

        momentumArrowX->setLabel("momentum x");
        momentumArrowZ->setLabel("momentum z");
        momentumArrowX->setPosition(descent->ball->position());
        momentumArrowZ->setPosition(descent->ball->position());

        temporary_ball->setVisible(false);
        total_arrow->setVisible(false);
        momentumArrowX->setLabelVisibility(!in_initial_state);
        momentumArrowZ->setLabelVisibility(!in_initial_state);

        break;
    }
    case 1: // decay the momentum
    {
        Point p = descent->position();
        AnimationHelper::setBallPositionOnSurface(descent->ball.get(), p);

        momentumArrowX->setMagnitude(momentumArrowX->magnitude() * descent->decay_rate);
        momentumArrowZ->setMagnitude(momentumArrowZ->magnitude() * descent->decay_rate);
        momentumArrowX->setLabel("decay momentum");
        momentumArrowZ->setLabel("decay momentum");
        momentumArrowX->setPosition(descent->ball->position());
        momentumArrowZ->setPosition(descent->ball->position());

        temporary_ball->setVisible(false);
        total_arrow->setVisible(false);
        momentumArrowX->setLabelVisibility(!in_initial_state);
        momentumArrowZ->setLabelVisibility(!in_initial_state);

        in_initial_state = false;

        break;
    }
    case 2: // show the x and z direction gradients
    {
        Point grad(descent->gradX(), descent->gradZ());
        arrowX->setMagnitude(grad.x);
        arrowZ->setMagnitude(grad.z);
        // if in the same direction, then start the arrow at the tip of the momentum arrow
        if (momentumArrowX->magnitude() * grad.x > 0){
            // hack: *0.95 to offset a little so the two arrows don't look disjointed
            arrowX->setPosition(descent->ball->position() + momentumArrowX->vector() * 0.95);
        }
        else{
            arrowX->setPosition(descent->ball->position());
        }

        if (momentumArrowZ->magnitude() * grad.z > 0){
            arrowZ->setPosition(descent->ball->position() + momentumArrowZ->vector() * 0.95);
        }
        else{
            arrowZ->setPosition(descent->ball->position());
        }

        momentumArrowX->setLabelVisibility(false);
        momentumArrowZ->setLabelVisibility(false);
        arrowX->setVisible(true);
        arrowZ->setVisible(true);
        break;
    }
    case 3: // show the composite of gradients
    {
        descent->takeGradientStep();
        Point delta = descent->delta();
        total_arrow->setVector(QVector3D(delta.x, 0, delta.z) / descent->learning_rate);
        total_arrow->setPosition(descent->ball->position());

        total_arrow->setVisible(true);
        break;
    }
    case 4: // show momentum for next iteration
    {
        Point delta = descent->delta();
        momentumArrowX->setMagnitude(-delta.x / descent->learning_rate);
        momentumArrowZ->setMagnitude(-delta.z / descent->learning_rate);
        momentumArrowX->setLabel("momentum for next iteration");
        momentumArrowZ->setLabel("momentum for next iteration");
        momentumArrowX->setPosition(descent->ball->position());
        momentumArrowZ->setPosition(descent->ball->position());

        arrowX->setVisible(false);
        arrowZ->setVisible(false);
        break;
    }
    case 5: // draw an imaginary ball of the future position
    {
        AnimationHelper::setBallPositionOnSurface(temporary_ball.get(),
                                         descent->position());
        temporary_ball->setPosition(QVector3D(
                                        descent->position().x,
                                        descent->ball->position().y(),
                                        descent->position().z));

        temporary_ball->setVisible(true);
        break;
    }
    }
}
