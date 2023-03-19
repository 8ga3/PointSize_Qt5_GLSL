#ifndef POINTCLOUDCUBE_H
#define POINTCLOUDCUBE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "viewgl.h"

class PointCloudCube : public QOpenGLFunctions
{
public:
    PointCloudCube();

    void drawPointCloudCubeGeometry(const QSize& size);
    void initialize();

    qreal pointSize() const {
        return m_fPointSize;
    }

    void setPointSize(qreal value) {
        m_fPointSize = value;
    }

    int pointCount() const {
        return m_nPointCount;
    }

    void setPointCount(int value) {
        if (m_nPointCount != value) {
            m_nPointCount = value;
            createPointCloud();
        }
    }

    ViewGL::ProjectionType projectionType() const {
        return m_projectionType;
    }

    void setProjectionType(ViewGL::ProjectionType value) {
        m_projectionType = value;
    }

    QMatrix4x4 sceneTransform() const {
        return m_sceneTransform;
    }

    void setSceneTransform(const QMatrix4x4 sceneTransform) {
        m_sceneTransform = sceneTransform;
    }

private:
    void initializeShaders();
    void createPointCloud();
    float randomFloat(const float lowest, const float highest);

    qreal   m_devicePixelRatio;
    qreal   m_fPointSize;
    int     m_nPointCount;
    ViewGL::ProjectionType m_projectionType;
    QMatrix4x4 m_sceneTransform;

    QVector<QVector3D> m_vertices;
    QOpenGLShaderProgram m_program;
    int m_vertexAttr;
    int m_matrixUniform;
};

#endif // POINTCLOUDCUBE_H
