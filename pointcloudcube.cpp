#include "pointcloudcube.h"
#include <QPainter>
#include <QPaintEngine>
#include <QGuiApplication>
#include <QScreen>
#include <QVector3D>
#include <QMatrix4x4>
#include <QRandomGenerator>

PointCloudCube::PointCloudCube()
    : m_fPointSize(1.0f), m_nPointCount(NUM_POINTS), m_projectionType(ViewGL::PERSPECTIVE)
{
    // retina
    QScreen *screen = QGuiApplication::primaryScreen();
    m_devicePixelRatio = screen->devicePixelRatio();
}

void PointCloudCube::initialize()
{
    initializeOpenGLFunctions();

    qDebug() << "Vendor: " << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    qDebug() << "GPU: " << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    qDebug() << "OpenGL version: " << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    qDebug() << "GSLS version: " << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    glClearColor(0.95f, 1.0f, 1.0f, 1.0f);

    initializeShaders();
}

void PointCloudCube::initializeShaders()
{
    const char *vsrc = R"(
        attribute highp vec3 vertex;
        uniform mediump mat4 matrix;
        varying mediump vec4 color;
        vec3 hsv(float h, float s, float v) {
            vec4 t = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            vec3 p = abs(fract(vec3(h) + t.xyz) * 6.0 - vec3(t.w));
            return v * mix(vec3(t.x), clamp(p - vec3(t.x), 0.0, 1.0), s);
        }
        void main()
        {
            float h = (vertex.y + 5.0) / 10.0;
            color = vec4(hsv(h, 0.8, 0.8), 1.0);
            gl_Position = matrix * vec4(vertex, 1.0);
        }
    )";

    const char *fsrc = R"(
        varying mediump vec4 color;
        void main()
        {
            gl_FragColor = color;
        }
    )";

    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
    m_program.link();

    m_vertexAttr = m_program.attributeLocation("vertex");
    m_matrixUniform = m_program.uniformLocation("matrix");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_fPointSize = 1.0f;
    createPointCloud();
}

void PointCloudCube::createPointCloud()
{
    m_vertices.clear();

    for (int i = 0; i < m_nPointCount; ++i) {
        float x = randomFloat(-5.0f, +5.0f);
        float y = randomFloat(-5.0f, +5.0f);
        float z = randomFloat(-5.0f, +5.0f);
        m_vertices << QVector3D(x, y, z);
    }
}

float PointCloudCube::randomFloat(const float lowest, const float highest)
{
    return lowest + QRandomGenerator::global()->generateDouble() * (highest - lowest);
}

void PointCloudCube::drawPointCloudCubeGeometry(const QSize& size)
{
    glDepthMask(true);

    glClearColor(0.95f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    const qreal zNear = 10.0f;
    const qreal zFar = 10000.0f;
    const qreal fov = 60.0f;
    const qreal aspect = qreal(size.width()) / qreal(size.height() ? size.height() : 1);

    const qreal eyeX = 0.0f;
    const qreal eyeY = 0.0f;
    const qreal eyeZ = -15.0f;

    const qreal targetX = 0.0f;
    const qreal targetY = 0.0f;
    const qreal targetZ = 0.0f;

    const qreal upX = 0.0f;
    const qreal upY = -1.0f;
    const qreal upZ = 0.0f;

    const qreal ORTHO_SIZE = 7;
    const qreal orthoWidth = ORTHO_SIZE * aspect;

    QMatrix4x4 modelview;

    if (m_projectionType == ViewGL::PERSPECTIVE) {
        modelview.perspective(fov, aspect, zNear, zFar);
    }
    else {
        modelview.ortho(-orthoWidth, orthoWidth, -ORTHO_SIZE, ORTHO_SIZE, zNear, zFar);
    }

    modelview.lookAt(
                QVector3D(eyeX, eyeY, eyeZ),
                QVector3D(targetX, targetY, targetZ),
                QVector3D(upX, upY, upZ));

    m_program.bind();
    m_program.setUniformValue(m_matrixUniform, modelview * m_sceneTransform);

    m_program.enableAttributeArray(m_vertexAttr);
    m_program.setAttributeArray(m_vertexAttr, m_vertices.constData());

    glPointSize(m_fPointSize * m_devicePixelRatio);

    glDrawArrays(GL_POINTS, 0, m_vertices.size());

    m_program.disableAttributeArray(m_vertexAttr);
    m_program.release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
