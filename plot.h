#ifndef PLOT_H
#define PLOT_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtCore/QTimer>
#include <gradientdescent.h>
#include <memory>
#include <vector>

using namespace QtDataVisualization;

class Plot : public QObject
{
    Q_OBJECT
public:
    explicit Plot(Q3DSurface *surface);
    ~Plot();
    std::unique_ptr<VanillaGradientDescent> gradient_descent;
    std::unique_ptr<Momentum> momemtum;
    std::unique_ptr<AdaGrad> ada_grad;
    std::unique_ptr<RMSProp> rms_prop;
    std::unique_ptr<Adam> adam;

public Q_SLOTS:
    void toggleAnimation();
    void triggerAnimation();
    void restartAnimation();
    void setAnimationSpeed(int index);
    void setCameraZoom(float zoom);
    void restartFromNewPosition(QPoint q_pos);
    void moveCamera(int x_direction, int z_direction);
    void cameraZoomIn();
    void cameraZoomOut();

private:

    std::vector<GradientDescent*> all_descents;
    QTimer m_timer;
    std::unique_ptr<Q3DSurface> m_graph;
    std::unique_ptr<QSurfaceDataProxy> m_surfaceProxy;
    std::unique_ptr<QSurface3DSeries> m_surfaceSeries;
    bool detailedView = true;
    GradientDescent* detailed_descent;

    float stepX;
    float stepZ;
    int timer_counter = 0;
    int animation_slowdown = 1; // slow down factor
    int animation_speedup = 1;  // speed up factor

    void initializeSurface();
    void initializeGraph();
    void initializeArrow(GradientDescent* descent);
};

#endif // PLOT_H
