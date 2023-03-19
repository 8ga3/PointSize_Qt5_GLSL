#ifndef VIEWGL_H
#define VIEWGL_H

#include <QQuickFramebufferObject>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

constexpr int NUM_POINTS = 10000;

class ViewGL : public QQuickFramebufferObject
{
public:
    enum ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(ViewGL)

    Q_ENUM(ProjectionType)
    Q_PROPERTY(QVector3D eulerRotation READ eulerRotation WRITE setEulerRotation NOTIFY eulerRotationChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D pivot READ pivot WRITE setPivot NOTIFY pivotChanged)
    Q_PROPERTY(QVector3D forward READ forward NOTIFY forwardChanged)
    Q_PROPERTY(QVector3D up READ up NOTIFY upChanged)
    Q_PROPERTY(QVector3D right READ right NOTIFY rightChanged)
    Q_PROPERTY(QMatrix4x4 sceneTransform READ sceneTransform NOTIFY sceneTransformChanged)
    Q_PROPERTY(qreal pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(int pointCount READ pointCount WRITE setPointCount NOTIFY pointCountChanged)
    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)

public:
    ViewGL(QQuickItem *parent = nullptr) : QQuickFramebufferObject(parent)
    {
    }

    QVector3D eulerRotation() const
    {
        return m_rotation;
    }

    QVector3D position() const
    {
        return m_position;
    }

    QVector3D scale() const
    {
        return m_scale;
    }

    QVector3D pivot() const
    {
        return m_pivot;
    }

    QVector3D forward() const
    {
        return mapDirectionToScene(QVector3D(0, 0, -1)).normalized();
    }

    QVector3D up() const
    {
        return mapDirectionToScene(QVector3D(0, 1, 0)).normalized();
    }

    QVector3D right() const
    {
        return mapDirectionToScene(QVector3D(1, 0, 0)).normalized();
    }

    QMatrix4x4 sceneTransform() const
    {
        return m_sceneTransform;
    }

    qreal pointSize() const
    {
        return m_fPointSize;
    }

    int pointCount() const
    {
        return m_nPointCount;
    }

    ViewGL::ProjectionType projectionType() const
    {
        return m_projectionType;
    }

//    void mousePressEvent(QMouseEvent *event) override;

    QQuickFramebufferObject::Renderer *createRenderer() const override;

public Q_SLOTS:
    void setEulerRotation(const QVector3D &eulerRotation) {
        if (m_rotation == eulerRotation)
            return;

        m_rotation = eulerRotation;
        calculateGlobalVariables();
        emit eulerRotationChanged();
        emit sceneTransformChanged();
        update();
    }

    void setPosition(const QVector3D &position)
    {
        if (m_position == position)
            return;

        m_position = position;
        calculateGlobalVariables();
        emit positionChanged();
        emit sceneTransformChanged();
        update();
    }

    void setScale(const QVector3D &scale)
    {
        if (m_scale == scale)
            return;

        m_scale = scale;
        calculateGlobalVariables();
        emit scaleChanged();
        emit sceneTransformChanged();
        update();
    }

    void setPivot(const QVector3D &pivot)
    {
        if (m_pivot == pivot)
            return;

        m_pivot = pivot;
        calculateGlobalVariables();
        emit pivotChanged();
        emit sceneTransformChanged();
        update();
    }

    void setPointSize(qreal value)
    {
        if (m_fPointSize != value) {
            m_fPointSize = value;
            update();
            emit pointSizeChanged();
        }
    }

    void setPointCount(int value)
    {
        if (m_nPointCount != value) {
            m_nPointCount = value;
            update();
            emit pointCountChanged();
        }
    }

    void setProjectionType(ViewGL::ProjectionType value)
    {
        if (m_projectionType != value) {
            m_projectionType = value;
            update();
            emit projectionTypeChanged();
        }
    }

Q_SIGNALS:
    void eulerRotationChanged();
    void positionChanged();
//    void scaleChanged(); // QQuickItem
    void pivotChanged();
    void forwardChanged();
    void upChanged();
    void rightChanged();
    void sceneTransformChanged();

    void pointSizeChanged();
    void pointCountChanged();
    void projectionTypeChanged();

private:
    QVector3D m_rotation;
    QVector3D m_position;
    QVector3D m_scale = QVector3D(1, 1, 1);
    QVector3D m_pivot;
    QMatrix4x4 m_sceneTransform;

    qreal m_fPointSize = 1.0f;
    int m_nPointCount = NUM_POINTS;
    ProjectionType m_projectionType;

    QVector3D mapDirectionToScene(const QVector3D &localDirection) const;
    QVector3D mat33_transform(const QMatrix3x3 &m, const QVector3D &v) const;
    QMatrix4x4 calculateTransformMatrix(QVector3D position, QVector3D scale, QVector3D pivot, QQuaternion rotation);
    void calculateGlobalVariables();
};

#endif // VIEWGL_H
