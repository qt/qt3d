/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtGui/qpainter.h>
#include <QtGui/QOpenGLContext>

#include "qgltestwidget.h"
#include "qglpainter.h"
#include "qglsimulator.h"

class tst_QGLPainter : public QObject
{
    Q_OBJECT
public:
    tst_QGLPainter() : widget(0) {}
    ~tst_QGLPainter() {}

private slots:
    void initTestCase();
    void cleanupTestCase();
    void clear();
    void drawTriangle();
    void scissor();
    void userMatrixStack();
    void projectionMatrixStack();
    void modelViewMatrixStack();
    void worldMatrix();
    void basicCullable();
    void isCullable_data();
    void isCullable();
    void isCullableVert_data();
    void isCullableVert();
    void lights();
    void nextPowerOfTwo_data();
    void nextPowerOfTwo();

public slots:
    void clearPaint();
    void clearPaintQ(QPainter *painter, const QSize& size);
    void drawTrianglePaint();
    void drawTrianglePaintQ(QPainter *painter, const QSize& size);
    void scissorPaint();
    void scissorPaintQ(QPainter *painter, const QSize& size);

private:
    QGLTestWidget *widget;
};

void tst_QGLPainter::initTestCase()
{
    QSKIP("QWidget: Cannot create a QWidget when no GUI is being used");
    widget = new QGLTestWidget();
}

void tst_QGLPainter::cleanupTestCase()
{
    delete widget;
}

void tst_QGLPainter::clear()
{
    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QVERIFY(widget->runTest(this, "clearPaint"));
}

void tst_QGLPainter::clearPaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::blue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void tst_QGLPainter::clearPaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);
    sim.setClearColor(Qt::blue);
    sim.clear();
}

void tst_QGLPainter::drawTriangle()
{
    QSKIP("Fails under qt5");
    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QVERIFY(widget->runTest(this, "drawTrianglePaint"));
}

void tst_QGLPainter::drawTrianglePaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projm;
    projm.ortho(widget->geometry());
    painter.projectionMatrix() = projm;
    painter.modelViewMatrix().setToIdentity();

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    painter.clearAttributes();
    painter.setStandardEffect(QGL::FlatColor);
    painter.setColor(Qt::green);
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.draw(QGL::Triangles, 3);
}

void tst_QGLPainter::drawTrianglePaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);

    sim.clear();
    sim.setColor(Qt::green);

    QMatrix4x4 proj;
    proj.ortho(widget->geometry());
    sim.setProjectionMatrix(proj);

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    sim.drawTriangles(vertices);
}

static QRect fetchGLScissor(const QRect& windowRect)
{
    GLint scissor[4];
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    if (scissor[2] != 0 && scissor[3] != 0) {
        return QRect(scissor[0],
                     windowRect.height() - (scissor[1] + scissor[3]),
                     scissor[2], scissor[3]);
    } else {
        // Normally should be (0, 0, 0, 0) - don't adjust for window height.
        return QRect(scissor[0], scissor[1], scissor[2], scissor[3]);
    }
}

void tst_QGLPainter::scissor()
{
    QSKIP("Does not pass on Qt5");

    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    // Run a painting test to check that the scissor works.
    QVERIFY(widget->runTest(this, "scissorPaint"));

    // Perform some lower level tests to ensure that the
    // GL state is updated properly as the scissor changes.
    QGLPainter painter;
    painter.begin(widget);
    QRect windowRect = widget->geometry();

    QVERIFY(!glIsEnabled(GL_SCISSOR_TEST));

    painter.setScissor(windowRect);
    QCOMPARE(fetchGLScissor(windowRect), windowRect);

    QRect subRect(windowRect.width() / 3,
                  windowRect.height() / 3,
                  2 * windowRect.width() / 3,
                  2 * windowRect.height() / 3);
    painter.setScissor(subRect);
    QCOMPARE(fetchGLScissor(windowRect), subRect);

    // Empty rectangles should result in glScissor(0, 0, 0, 0).
    painter.setScissor(QRect(0, 0, -2, -2));
    QCOMPARE(fetchGLScissor(windowRect), QRect(0, 0, 0, 0));
    painter.setScissor(QRect(27, 42, 0, 0));
    QCOMPARE(fetchGLScissor(windowRect), QRect(0, 0, 0, 0));
}

void tst_QGLPainter::scissorPaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projm;
    projm.ortho(widget->geometry());
    painter.projectionMatrix() = projm;
    painter.modelViewMatrix().setToIdentity();

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    // Paint a green triangle.
    painter.clearAttributes();
    painter.setStandardEffect(QGL::FlatColor);
    painter.setColor(Qt::green);
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.draw(QGL::Triangles, 3);

    // Change the top part of the triangle to blue.
    QRect scissor(0, 0, widget->width(), qMin(widget->height() / 2, 200));
    painter.setScissor(scissor);
    glEnable(GL_SCISSOR_TEST);
    painter.setColor(Qt::blue);
    painter.draw(QGL::Triangles, 3);

    // Intersect and draw red over the blue section.
    painter.setScissor(scissor.intersected
        (QRect(0, 0, widget->width(), qMin(widget->height() / 4, 150))));
    painter.setColor(Qt::red);
    painter.draw(QGL::Triangles, 3);

    // Change the bottom part of the triangle to yellow.
    int y = qMin(widget->height() / 2, 350);
    scissor = QRect(0, y, 400, widget->height() - y);
    painter.setScissor(scissor);
    painter.setColor(Qt::yellow);
    painter.draw(QGL::Triangles, 3);

    // Intersect and expand, to extend the yellow region.
    scissor &= QRect(0, y + 20, 400, widget->height() - y - 20);
    scissor |= QRect(0, y + 20, 450, widget->height() - y - 20);
    painter.setScissor(scissor);
    painter.setColor(Qt::yellow);
    painter.draw(QGL::Triangles, 3);

    glDisable(GL_SCISSOR_TEST);
}

void tst_QGLPainter::scissorPaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);

    sim.clear();

    QMatrix4x4 proj;
    proj.ortho(widget->geometry());
    sim.setProjectionMatrix(proj);

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    // Paint a green triangle.
    sim.setColor(Qt::green);
    sim.drawTriangles(vertices);

    // Change the top part of the triangle to blue.
    sim.setScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 2, 200)));
    sim.setColor(Qt::blue);
    sim.drawTriangles(vertices);

    // Intersect and draw red over the blue section.
    sim.intersectScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 4, 150)));
    sim.setColor(Qt::red);
    sim.drawTriangles(vertices);

    // Change the bottom part of the triangle to yellow.
    int y = qMin(widget->height() / 2, 350);
    sim.setScissor
        (QRect(0, y, 400, widget->height() - y));
    sim.setColor(Qt::yellow);
    sim.drawTriangles(vertices);

    // Intersect and expand, to extend the yellow region.
    sim.intersectScissor
        (QRect(0, y + 20, 400, widget->height() - y - 20));
    sim.expandScissor
        (QRect(0, y + 20, 450, widget->height() - y - 20));
    sim.setColor(Qt::yellow);
    sim.drawTriangles(vertices);
}

// Test the mathematical correctness of matrix stacks on a user stack.
// We assume that QMatrix4x4 works and we can use it as an oracle.
void tst_QGLPainter::userMatrixStack()
{
    QMatrix4x4Stack stack;

    QVERIFY(stack.top().isIdentity());
    QVERIFY(QMatrix4x4(stack).isIdentity());

    QMatrix4x4 m;
    m.translate(1, 2, 3);
    m.scale(4, 5, 6);
    m.rotate(90, 0, 1, 0);

    stack.translate(1, 2, 3);
    stack.scale(4, 5, 6);
    stack.rotate(90, 0, 1, 0);
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.push();
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.setToIdentity();
    QVERIFY(stack.top().isIdentity());

    stack.translate(QVector3D(1, 2, 3));
    stack.scale(QVector3D(4, 5, 6));
    stack.rotate(90, QVector3D(0, 1, 0));
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    QMatrix4x4 m2(m);
    m2.scale(-3);

    stack.scale(-3);
    QVERIFY(qFuzzyCompare(m2, stack.top()));
    QVERIFY(qFuzzyCompare(m2, QMatrix4x4(stack)));

    stack.pop();

    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.pop(); // at bottom of stack - no change

    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack = m2;
    QVERIFY(qFuzzyCompare(m2, stack.top()));
    QVERIFY(qFuzzyCompare(m2, QMatrix4x4(stack)));

    QMatrix4x4 m3 = m2 * m;
    stack *= m;
    QVERIFY(qFuzzyCompare(m3, stack.top()));
    QVERIFY(qFuzzyCompare(m3, QMatrix4x4(stack)));

    m3.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    stack.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));

    QVERIFY(qFuzzyCompare(m3, stack.top()));
    QVERIFY(qFuzzyCompare(m3, QMatrix4x4(stack)));

    QMatrix4x4 m4;

    m4.setToIdentity();
    m4.ortho(QRect(2, 3, 4, 5));
    stack.setToIdentity();
    QMatrix4x4 projm;
    projm.ortho(QRect(2, 3, 4, 5));
    stack = projm;
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));
}

#if defined(QT_OPENGL_ES_2)
#define QGL_NO_MATRIX_FETCH 1
#endif

#ifndef QGL_NO_MATRIX_FETCH

static void clearGLMatrix(GLenum type)
{
    glMatrixMode(type);
    glLoadIdentity();
}

static bool checkGLMatrix(GLenum type, const QMatrix4x4& expected)
{
    QMatrix4x4 actual;
    int index;
    GLfloat mat[16];
    glGetFloatv(type, mat);
    float *m = actual.data();
    for (index = 0; index < 16; ++index)
        m[index] = mat[index];
    for (index = 0; index < 16; ++index) {
        // Introduce some fuzziness for GL servers that
        // use a different precision from QMatrix4x4.
        if (qAbs(actual.constData()[index] - expected.constData()[index])
                >= 0.001)
            return false;
    }
    return true;
}

#else

#ifndef GL_PROJECTION
#define GL_PROJECTION           0
#endif
#ifndef GL_PROJECTION_MATRIX
#define GL_PROJECTION_MATRIX    0
#endif
#ifndef GL_MODELVIEW
#define GL_MODELVIEW            0
#endif
#ifndef GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX     0
#endif

// OpenGL/ES 2.0 does not have server-side matrices.
// For such platforms, we stub out the checks and just hope that they work.
static void clearGLMatrix(GLenum) {}
static bool checkGLMatrix(GLenum, const QMatrix4x4&) { return true; }

#endif

void tst_QGLPainter::projectionMatrixStack()
{
    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter painter(widget);

    painter.projectionMatrix().setToIdentity();
    QVERIFY(painter.projectionMatrix().top().isIdentity());
    QVERIFY(QMatrix4x4(painter.projectionMatrix()).isIdentity());

    // Clear the matrix in the GL server.
    clearGLMatrix(GL_PROJECTION);

    QMatrix4x4 m;
    m.ortho(2, 4, 3, 1, 10, 50);
    painter.projectionMatrix() = m;
    QVERIFY(qFuzzyCompare(m, painter.projectionMatrix().top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(painter.projectionMatrix())));

    // The matrix in the GL server should still be the identity.
    QVERIFY(checkGLMatrix(GL_PROJECTION_MATRIX, QMatrix4x4()));

    // Force an update to the GL server.
    painter.updateFixedFunction(QGLPainter::UpdateProjectionMatrix);

    // Check that the server received the value we set.
    QVERIFY(checkGLMatrix(GL_PROJECTION_MATRIX, m));
}

void tst_QGLPainter::modelViewMatrixStack()
{
    if (!widget->context()->isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter painter(widget);

    painter.modelViewMatrix().setToIdentity();
    QVERIFY(painter.modelViewMatrix().top().isIdentity());
    QVERIFY(QMatrix4x4(painter.modelViewMatrix()).isIdentity());

    // Clear the matrix in the GL server.
    clearGLMatrix(GL_MODELVIEW);

    QMatrix4x4 m;
    m.translate(1, 2, 3);
    m.scale(4, 5, 6);
    m.rotate(90, 0, 1, 0);
    painter.modelViewMatrix().translate(1, 2, 3);
    painter.modelViewMatrix().scale(4, 5, 6);
    painter.modelViewMatrix().rotate(90, 0, 1, 0);
    QVERIFY(qFuzzyCompare(m, painter.modelViewMatrix().top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(painter.modelViewMatrix())));

    // The matrix in the GL server should still be the identity.
    QVERIFY(checkGLMatrix(GL_MODELVIEW_MATRIX, QMatrix4x4()));

    // Force an update to the GL server.
    painter.updateFixedFunction(QGLPainter::UpdateModelViewMatrix);

    // Check that the server received the value we set.
    QVERIFY(checkGLMatrix(GL_MODELVIEW_MATRIX, m));
}

static bool fuzzyCompare(const QMatrix4x4 &m1, const QMatrix4x4 &m2)
{
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (qAbs(m1(row, col) - m2(row, col)) >= 0.00001f)
                return false;
        }
    }
    return true;
}

void tst_QGLPainter::worldMatrix()
{
    QGLPainter painter(widget);

    QGLCamera camera;
    camera.setEye(QVector3D(1, 2, 10));
    camera.setCenter(QVector3D(20, 3, -4));
    camera.setUpVector(QVector3D(1, 1, 1));
    painter.setCamera(&camera);

    // The modelview matrix should be the eye look-at component.
    QMatrix4x4 mv;
    mv.lookAt(camera.eye(), camera.center(), camera.upVector());
    QVERIFY(fuzzyCompare(painter.modelViewMatrix(), mv));

    // The world matrix should currently be the identity.
    QVERIFY(fuzzyCompare(painter.worldMatrix(), QMatrix4x4()));

    // Change the modelview.
    painter.modelViewMatrix().translate(0.0f, 5.0f, 0.0f);
    painter.modelViewMatrix().scale(1.5f);

    // Check the modelview and world matrices against their expected values.
    QMatrix4x4 world;
    world.translate(0.0f, 5.0f, 0.0f);
    world.scale(1.5f);
    QVERIFY(fuzzyCompare(painter.modelViewMatrix(), mv * world));
    QVERIFY(fuzzyCompare(painter.worldMatrix(), world));
}

static void ensureContext(QWindow &win, QOpenGLContext &ctx)
{
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    ctx.setFormat(format);
#ifndef QT_NO_DEBUG_STREAM
    QSurfaceFormat oldFormat = format;
#endif
    ctx.create();
    // TODO: is it possible that the platform will downgrade the actual
    // format, or will it just fail if it can't deliver the actual format
    format = ctx.format();
#ifndef QT_NO_DEBUG_STREAM
    if (oldFormat.swapBehavior() != format.swapBehavior())
        qWarning() << "Could not create context for swap behavior"
                   << oldFormat.swapBehavior();
#endif
    ctx.makeCurrent(&win);
}

void tst_QGLPainter::basicCullable()
{
    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter painter(&glw);
    painter.modelViewMatrix().setToIdentity();

    // Default values for the camera have
    // eye = (0, 0, 10) - so eye at positive 10 on z-axis
    // center = (0, 0, 0) - looking at the origin, in a negative z-direction
    // viewport = 2 x 2 - ideal square view frustum
    // near = 5 - distance from eye to near plane
    // far = 1000 - distance from eye to far plane
    // far plane is a z = -990
    QGLCamera camera;
    painter.setCamera(&camera);

    // origin itself and a point 10 further down the view vector
    // both must be visible
    QVERIFY(!painter.isCullable(QVector3D(0, 0, 0)));
    QVERIFY(!painter.isCullable(QVector3D(0, 0, -10)));

    // the position of the eye itself, is behind the near plane, so culled.
    QVERIFY(painter.isCullable(QVector3D(0, 0, 10)));
    // Reset the camera and then check boxes in front of the camera
    // that are close to the near and far planes and the eye.
    QGLCamera camera2;
    painter.setCamera(&camera2);

    // Box around the eye, but in front of the near plane.
    QBox3D box2(QVector3D(-1, -1, 11), QVector3D(1, 1, 9));
    QVERIFY(painter.isCullable(box2));

    // Box that surrounds the near plane.
    QBox3D box3(QVector3D(-1, -1, 6), QVector3D(1, 1, 4));
    QVERIFY(!painter.isCullable(box3));

    // Box between the near plane and the eye.
    QBox3D box4(QVector3D(-1, -1, 7), QVector3D(1, 1, 6));
    QVERIFY(painter.isCullable(box4));

    // Box extending from behind the eye to inside the viewing cube.
    // Not cullable because it is partially visible.
    QBox3D box5(QVector3D(-1, -1, 11), QVector3D(1, 1, 0));
    QVERIFY(!painter.isCullable(box5));

    // Box that surrounds the far plane.
    QBox3D box6(QVector3D(-1, -1, -989), QVector3D(1, 1, -991));
    QVERIFY(!painter.isCullable(box6));

    // Box that is between the near and far planes, but close to far plane.
    QBox3D box7(QVector3D(-1, -1, -988), QVector3D(1, 1, -989));
    QVERIFY(!painter.isCullable(box7));

    // Box that is beyond the far plane.
    QBox3D box8(QVector3D(-1, -1, -1000), QVector3D(1, 1, -1001));
    QVERIFY(painter.isCullable(box8));
}

static void setupTestCamera(QGLCamera &camera, float distance)
{
    // These values are based on the special camera as per the images in
    // doc/src/images/view-frustum*

    // Check the cullability of a box at 10 degree increments of rotation.
    // It should be visible between -20 and 20 degrees but otherwise not.
    // Also check the center point of the box.

    camera.setNearPlane(2.2f);
    camera.setFarPlane(14.0f);

    // This value comes from the ratio of the far plane size 8 x 8 to the
    // far plane distance, multiplied by the near plane, to give the near
    // plane size:  2.2 x (4 / 14) = 1.257
    camera.setViewSize(QSizeF(1.257f, 1.257f));

    // as shown in the images, we use a camera at -z instead of the default +z
    // but the center remains the same at 0, 0, 0
    camera.setEye(QVector3D(0, 0, distance));

    // if the distance would push the camera eye past the view center
    // move the view center away
    if (distance > 0.0f)
        camera.setCenter(QVector3D(0.0f, 0.0f, distance + 2));
}

void tst_QGLPainter::isCullable_data()
{
    QTest::addColumn<float>("angle");
    QTest::addColumn<float>("distance");
    QTest::addColumn<bool>("culled");
    QTest::addColumn<bool>("center_culled");

    // Check the cullability of a box at 10 degree increments of rotation.
    // It should be visible between -20 and 20 degrees but otherwise not.
    // Also check the point at the center of the box.

    QTest::newRow("culled-neg40") << -40.0f << -8.0f << true << true;
    QTest::newRow("culled-neg30") << -30.0f << -8.0f << true << true;
    QTest::newRow("part-vis-neg20") << -20.0f << -8.0f << false << false;
    QTest::newRow("part-vis-neg10") << -10.0f << -8.0f << false << false;
    QTest::newRow("all-vis-00") << 0.0f << -8.0f << false << false;
    QTest::newRow("part-vis-pos10") << 10.0f << -8.0f << false << false;
    QTest::newRow("part-vis-pos20") << 20.0f << -8.0f << false << false;
    QTest::newRow("culled-pos30") << 30.0f << -8.0f << true << true;
    QTest::newRow("culled-pos40") << 40.0f << -8.0f << true << true;

    // Check when the eye is at varying distances from the box, with
    // (in one case with an angle for good measure) so that it needs
    // testing against the near and far planes.

    QTest::newRow("culled-eye-z+2") << 0.0f << 2.0f << true << true;
    QTest::newRow("culled-eye-z+0") << 0.0f << 0.0f << true << true;
    QTest::newRow("part-vis-eye-z-2-around-np") << 0.0f << -2.0f << false << true;
    QTest::newRow("part-vis-eye-z-3-around-np") << 0.0f << -3.0f << false << false;
    QTest::newRow("part-vis-eye-z-4-close-np") << 0.0f << -4.0f << false << false;
    QTest::newRow("part-vis-eye-z-14.5") << 0.0f << -14.5f << false << true;
    QTest::newRow("part-vis-eye-z-14-angle-20") << 20.0f << -14.0f << false << false;
    QTest::newRow("culled-eye-z-20") << 0.0f << -20.0f << true << true;
}

void tst_QGLPainter::isCullable()
{
    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter painter(&glw);
    painter.modelViewMatrix().setToIdentity();

    QGLCamera camera;

    QFETCH(float, angle);
    QFETCH(float, distance);
    QFETCH(bool, culled);
    QFETCH(bool, center_culled);

    setupTestCamera(camera, distance);
    if (angle != 0.0f)
        camera.rotateEye(camera.pan(angle));

    painter.setCamera(&camera);

    QBox3D box(QVector3D(-1, -1, -1), QVector3D(1, 1, 1));
    QCOMPARE(painter.isCullable(box), culled);
    QCOMPARE(painter.isCullable(box.center()), center_culled);
}

void tst_QGLPainter::isCullableVert_data()
{
    QTest::addColumn<float>("angle");
    QTest::addColumn<float>("distance");
    QTest::addColumn<bool>("culled");
    QTest::addColumn<bool>("center_culled");

    // Check the cullability of a box at 10 degree increments of rotation.
    // It should be visible between -20 and 20 degrees but otherwise not.
    // Also check the point at the center of the box.

    QTest::newRow("culled-neg40") << -40.0f << -8.0f << true << true;
    QTest::newRow("culled-neg30") << -30.0f << -8.0f << true << true;
    QTest::newRow("part-vis-neg20") << -20.0f << -8.0f << false << true;
    QTest::newRow("part-vis-neg10") << -10.0f << -8.0f << false << false;
    QTest::newRow("all-vis-00") << 0.0f << -8.0f << false << false;
    QTest::newRow("part-vis-pos10") << 10.0f << -8.0f << false << false;
    QTest::newRow("part-vis-pos20") << 20.0f << -8.0f << false << true;
    QTest::newRow("culled-pos30") << 30.0f << -8.0f << true << true;
    QTest::newRow("culled-pos40") << 40.0f << -8.0f << true << true;

    QTest::newRow("culled-eye-z+2") << 0.0f << 2.0f << true << true;
    QTest::newRow("culled-eye-z+0") << 0.0f << 0.0f << true << true;
    QTest::newRow("part-vis-eye-z-2-around-np") << 0.0f << -2.0f << false << true;
    QTest::newRow("part-vis-eye-z-3-around-np") << 0.0f << -3.0f << false << false;
    QTest::newRow("part-vis-eye-z-4-close-np") << 0.0f << -4.0f << false << false;
    QTest::newRow("part-vis-eye-z-14.2") << 0.0f << -14.2f << false << true;
    QTest::newRow("part-vis-eye-z-14-angle-20") << 20.0f << -14.0f << false << true;
    QTest::newRow("culled-eye-z-20") << 0.0f << -20.0f << true << true;
}

void tst_QGLPainter::isCullableVert()
{
    //QSKIP("isCullableVert not passing currently");
    // same test as isCullable, except we tilt the camera forward
    // and back instead of rotating it from side to side
    // since the view frustum is typically not square in section
    // as it depends on the surface, this will give different results.
    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter painter(&glw);
    painter.modelViewMatrix().setToIdentity();

    QGLCamera camera;

    QFETCH(float, angle);
    QFETCH(float, distance);
    QFETCH(bool, culled);
    QFETCH(bool, center_culled);

    setupTestCamera(camera, distance);
    if (angle != 0.0f)
        camera.rotateEye(camera.tilt(angle));

    painter.setCamera(&camera);

    QBox3D box(QVector3D(-1, -1, -1), QVector3D(1, 1, 1));
    QCOMPARE(painter.isCullable(box), culled);
    QCOMPARE(painter.isCullable(box.center()), center_culled);
}

void tst_QGLPainter::lights()
{
    QGLPainter painter(widget);

    // Make sure the mainLight() is not present before we start these tests.
    painter.removeLight(0);

    QCOMPARE(painter.maximumLightId(), -1);
    QVERIFY(painter.light(0) == 0);
    QVERIFY(painter.lightTransform(0).isIdentity());
    QVERIFY(painter.light(-1) == 0);
    QVERIFY(painter.lightTransform(-1).isIdentity());

    QGLLightParameters lparams1;
    QGLLightParameters lparams2;
    QGLLightParameters lparams3;

    int lightId1 = painter.addLight(&lparams1);
    QCOMPARE(lightId1, 0);
    QCOMPARE(painter.maximumLightId(), 0);
    QVERIFY(painter.light(lightId1) == &lparams1);
    QVERIFY(painter.lightTransform(lightId1) == painter.modelViewMatrix());

    QMatrix4x4 m(painter.modelViewMatrix());
    m.translate(-1, 2, -5);

    int lightId2 = painter.addLight(&lparams2, m);
    QCOMPARE(lightId2, 1);
    QCOMPARE(painter.maximumLightId(), 1);
    QVERIFY(painter.light(lightId1) == &lparams1);
    QVERIFY(painter.lightTransform(lightId1) == painter.modelViewMatrix());
    QVERIFY(painter.light(lightId2) == &lparams2);
    QVERIFY(painter.lightTransform(lightId2) == m);

    painter.removeLight(lightId1);
    QCOMPARE(painter.maximumLightId(), 1);
    QVERIFY(painter.light(lightId1) == 0);
    QVERIFY(painter.lightTransform(lightId1).isIdentity());
    QVERIFY(painter.light(lightId2) == &lparams2);
    QVERIFY(painter.lightTransform(lightId2) == m);

    int lightId3 = painter.addLight(&lparams3);
    QCOMPARE(lightId3, 0);
    QCOMPARE(painter.maximumLightId(), 1);
    QVERIFY(painter.light(lightId3) == &lparams3);
    QVERIFY(painter.lightTransform(lightId3) == painter.modelViewMatrix());
    QVERIFY(painter.light(lightId2) == &lparams2);
    QVERIFY(painter.lightTransform(lightId2) == m);

    painter.removeLight(lightId2);
    QCOMPARE(painter.maximumLightId(), 0);
    QVERIFY(painter.light(lightId3) == &lparams3);
    QVERIFY(painter.lightTransform(lightId3) == painter.modelViewMatrix());
    QVERIFY(painter.light(lightId2) == 0);
    QVERIFY(painter.lightTransform(lightId2).isIdentity());

    painter.removeLight(lightId3);
    QCOMPARE(painter.maximumLightId(), -1);
    QVERIFY(painter.light(lightId3) == 0);
    QVERIFY(painter.lightTransform(lightId3).isIdentity());
    QVERIFY(painter.light(lightId2) == 0);
    QVERIFY(painter.lightTransform(lightId2).isIdentity());

    // Check default construction of the main light.
    const QGLLightParameters *mainLight = painter.mainLight();
    QVERIFY(mainLight != 0);
    QCOMPARE(painter.maximumLightId(), 0);
    QVERIFY(painter.light(0) == mainLight);
    QVERIFY(painter.lightTransform(0).isIdentity());
}

void tst_QGLPainter::nextPowerOfTwo_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<int>("result");

    QTest::newRow("0") << 0 << 0;
    QTest::newRow("1") << 1 << 1;
    QTest::newRow("2") << 2 << 2;
    QTest::newRow("3") << 3 << 4;
    QTest::newRow("4") << 4 << 4;
    QTest::newRow("5") << 5 << 8;
    QTest::newRow("6") << 6 << 8;
    QTest::newRow("7") << 7 << 8;
    QTest::newRow("13") << 13 << 16;
    QTest::newRow("16") << 16 << 16;
    QTest::newRow("23") << 23 << 32;
    QTest::newRow("32") << 32 << 32;
    QTest::newRow("63") << 63 << 64;
    QTest::newRow("64") << 64 << 64;
    QTest::newRow("65") << 65 << 128;
    QTest::newRow("120") << 120 << 128;
    QTest::newRow("128") << 128 << 128;
    QTest::newRow("129") << 129 << 256;

    for (int bit = 8; bit <= 30; ++bit) {
        int value = (1 << bit);
        QByteArray str;
        str = QByteArray::number(value);
        QTest::newRow(str.constData()) << (value - 23) << value;
        QTest::newRow(str.constData()) << value << value;
    }
}

void tst_QGLPainter::nextPowerOfTwo()
{
    QFETCH(int, value);
    QFETCH(int, result);

    QCOMPARE(QGL::nextPowerOfTwo(value), result);
    QCOMPARE(QGL::nextPowerOfTwo(QSize(value, 0)), QSize(result, 0));
    QCOMPARE(QGL::nextPowerOfTwo(QSize(0, value)), QSize(0, result));
}

QTEST_MAIN(tst_QGLPainter)

#include "tst_qglpainter.moc"
