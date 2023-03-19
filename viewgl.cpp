#include "viewgl.h"
#include "pointcloudcube.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

class PointCloudCubeRenderer : public QQuickFramebufferObject::Renderer
{
public:
    PointCloudCubeRenderer()
    {
        geometries.initialize();
    }

    void render() override {
        geometries.drawPointCloudCubeGeometry(m_size);
        update();
    }

    void synchronize(QQuickFramebufferObject *item) override {
        ViewGL* p = (ViewGL*)item;
        QVariant v = p->property("pointSize");
        geometries.setPointSize(v.toReal());
        QVariant count = p->property("pointCount");
        geometries.setPointCount(count.toInt());
        QVariant proj = p->property("projectionType");
        geometries.setProjectionType((ViewGL::ProjectionType)proj.toInt());
        QVariant transform = p->property("sceneTransform");
        geometries.setSceneTransform(transform.value<QMatrix4x4>());
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        m_size = size;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    PointCloudCube geometries;
    QSize m_size;
};

//void ViewGL::mousePressEvent(QMouseEvent *event)
//{
//    qDebug() << event->x() << ", " << event->y();
//}

QQuickFramebufferObject::Renderer *ViewGL::createRenderer() const
{
    return new PointCloudCubeRenderer();
}

QVector3D ViewGL::mapDirectionToScene(const QVector3D &localDirection) const
{
    QMatrix3x3 theDirMatrix = m_sceneTransform.normalMatrix();
    return mat33_transform(theDirMatrix, localDirection);
}

QVector3D ViewGL::mat33_transform(const QMatrix3x3 &m, const QVector3D &v) const
{
    const QVector3D c0 = QVector3D(m(0, 0), m(1, 0), m(2, 0));
    const QVector3D c1 = QVector3D(m(0, 1), m(1, 1), m(2, 1));
    const QVector3D c2 = QVector3D(m(0, 2), m(1, 2), m(2, 2));
    return c0 * v.x() + c1 * v.y() + c2 * v.z();
}

QMatrix4x4 ViewGL::calculateTransformMatrix(QVector3D position, QVector3D scale, QVector3D pivot, QQuaternion rotation)
{
    QMatrix4x4 transform;

    // Offset the origin (this is our pivot point)
    auto offset = (-pivot * scale);

    // Scale
    transform(0, 0) = scale[0];
    transform(1, 1) = scale[1];
    transform(2, 2) = scale[2];

    // Offset (before rotation)
    transform(0, 3) = offset[0];
    transform(1, 3) = offset[1];
    transform(2, 3) = offset[2];

    // rotate
    transform = QMatrix4x4{rotation.toRotationMatrix()} * transform;

    // translate
    transform(0, 3) += position[0];
    transform(1, 3) += position[1];
    transform(2, 3) += position[2];

    return transform;
}

void ViewGL::calculateGlobalVariables()
{
    QQuaternion quaternion = QQuaternion::fromEulerAngles(m_rotation);
    m_sceneTransform = calculateTransformMatrix(m_position, m_scale, m_pivot, quaternion);
}
