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

#include "qglpainter.h"
#include "qglbuilder.h"
#include "qglcube.h"
#include "qglteapot.h"
#include "qglcamera.h"
#include "qgltexture2d.h"
#include "qglsubsurface.h"

#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QtGui/QFontDatabase>

#include <QWindow>
#include <QOpenGLContext>
#include <QSurfaceFormat>

class ShapesWidget : public QWindow
{
    Q_OBJECT
public:
    ShapesWidget(QWindow *parent = 0);
    ~ShapesWidget();

protected:
    void exposeEvent(QExposeEvent *);

    void resizeGL(int w, int h);
    void initializeGL();
    void paintGL();

private:
    void ensureContext();

    void paintPoints(QGLPainter *painter, const QRect& rect);
    void paintLines(QGLPainter *painter, const QRect& rect);
    void paintLineStrip(QGLPainter *painter, const QRect& rect);
    void paintLineLoop(QGLPainter *painter, const QRect& rect);
    void paintTriangles(QGLPainter *painter, const QRect& rect);
    void paintTriangleStrip(QGLPainter *painter, const QRect& rect);
    void paintTriangleFan(QGLPainter *painter, const QRect& rect);
    void paintCube(QGLPainter *painter, const QRect& rect);
    void paintTeapot(QGLPainter *painter, const QRect& rect);
    void drawText(QGLPainter *painter, const QRect& posn, const QString& str);

    static QVector2DArray basicPoints(const QRect& rect);

    QGLCamera camera;
    QGLSceneNode *scene;
    QGLSceneNode *cube;
    QGLSceneNode *teapot;
    QGLLightModel oneSidedModel;
    QGLLightModel twoSidedModel;

    QOpenGLContext *context;
    bool initialised;
    QSurfaceFormat format;
};

ShapesWidget::ShapesWidget(QWindow *parent)
    : QWindow(parent)
    , scene(0)
    , cube(0)
    , teapot(0)
    , context(0)
    , initialised(false)
{
    setTitle(tr("GL Primitive Shapes"));
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(24);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);

    oneSidedModel.setModel(QGLLightModel::OneSided);
    twoSidedModel.setModel(QGLLightModel::TwoSided);
}

ShapesWidget::~ShapesWidget()
{
    delete scene;
}

void ShapesWidget::exposeEvent(QExposeEvent *e)
{
    Q_UNUSED(e);

    ensureContext();

    QRect rect = geometry();
    resizeGL(rect.width(), rect.height());

    if (!initialised)
        initializeGL();

    paintGL();

    context->swapBuffers(this);
}

void ShapesWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ShapesWidget::initializeGL()
{
    QGLPainter painter;
    painter.begin();

    painter.setLightModel(&twoSidedModel);
    painter.setFaceColor(QGL::FrontFaces, QColor(170, 202, 0));
    painter.setFaceColor(QGL::BackFaces, QColor(202, 170, 0));

    QGLBuilder builder;
    builder << QGL::Faceted << QGLCube();
    cube = builder.currentNode();
    builder << QGL::Smooth << QGLTeapot();
    teapot = builder.currentNode();
    scene = builder.finalizedSceneNode();

    initialised = true;
}

void ShapesWidget::paintGL()
{
    QGLPainter painter(this);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    QRect rect = geometry();
    QRect rectnew(0,0,rect.width(),rect.height());

    QMatrix4x4 projm;
    projm.ortho(rectnew);
    painter.projectionMatrix() = projm;
    painter.modelViewMatrix().setToIdentity();

    QRect wrect = geometry();
    int boxw = wrect.width() / 3;
    int boxh = wrect.height() / 3;

    paintLines(&painter, QRect(0, 0, boxw, boxh));
    paintLineStrip(&painter, QRect(boxw, 0, boxw, boxh));
    paintLineLoop(&painter, QRect(boxw * 2, 0, boxw, boxh));
    paintTriangles(&painter, QRect(0, boxh, boxw, boxh));
    paintTriangleStrip(&painter, QRect(boxw, boxh, boxw, boxh));
    paintTriangleFan(&painter, QRect(boxw * 2, boxh, boxw, boxh));
    paintPoints(&painter, QRect(0, boxh * 2, boxw, boxh));

    glEnable(GL_DEPTH_TEST);

    paintCube(&painter, QRect(boxw, boxh * 2, boxw, boxh));
    paintTeapot(&painter, QRect(boxw * 2, boxh * 2, boxw, boxh));
}

QVector2DArray ShapesWidget::basicPoints(const QRect& rect)
{
    QVector2DArray vertices;
    int step = qMin(rect.width() / 6, rect.height() / 6);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;
    vertices.append(midx - step * 2, midy - step);
    vertices.append(midx + step, midy - step * 2);
    vertices.append(midx, midy - step);
    vertices.append(midx + step * 2 + step / 2, midy - step / 2);
    vertices.append(midx - step, midy);
    vertices.append(midx + step * 2, midy + step);
    vertices.append(midx - step, midy + step);
    vertices.append(midx + step * 2, midy);
    return vertices;
}

void ShapesWidget::paintPoints(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(170, 202, 0));

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::Points, 8);

    drawText(painter, rect, tr("Points"));
}

void ShapesWidget::paintLines(QGLPainter *painter, const QRect& rect)
{
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(170, 202, 0));
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::Lines, 8);

    drawText(painter, rect, tr("Lines"));
}

void ShapesWidget::paintLineStrip(QGLPainter *painter, const QRect& rect)
{
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(170, 202, 0));
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::LineStrip, 8);

    drawText(painter, rect, tr("Line strip"));
}

void ShapesWidget::paintLineLoop(QGLPainter *painter, const QRect& rect)
{
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(170, 202, 0));
    painter->setVertexAttribute(QGL::Position, basicPoints(rect));
    painter->draw(QGL::LineLoop, 8);

    drawText(painter, rect, tr("Line loop"));
}

void ShapesWidget::paintTriangles(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    QVector3DArray normals;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step * 3, midy);
    vertices.append(midx - step * 3, midy + step * 2);
    vertices.append(midx - step, midy + step * 2);

    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);

    vertices.append(midx - step * 2, midy - step);
    vertices.append(midx + step / 2, midy + step * 2);
    vertices.append(midx + step * 3, midy - step);

    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::Normal, normals);
    painter->draw(QGL::Triangles, 6);

    drawText(painter, rect, tr("Triangles"));
}

void ShapesWidget::paintTriangleStrip(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    QVector3DArray normals;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step * 3, midy - step * 2);
    vertices.append(midx - step * 3, midy + step);
    vertices.append(midx - step, midy - step * 2);
    vertices.append(midx, midy + step);
    vertices.append(midx + step, midy - step * 2);
    vertices.append(midx + step * 2, midy + step * 2);

    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::Normal, normals);
    painter->draw(QGL::TriangleStrip, 6);

    // Overpaint some lines to show the triangle boundaries.
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(202, 170, 0));
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->draw(QGL::LineStrip, 4, 1);

    drawText(painter, rect, tr("Triangle strip"));
}

void ShapesWidget::paintTriangleFan(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    QVector2DArray vertices;
    QVector3DArray normals;
    int step = qMin(rect.width() / 8, rect.height() / 8);
    int midx = rect.x() + rect.width() / 2;
    int midy = rect.y() + rect.height() / 2;

    vertices.append(midx - step, midy + step);
    vertices.append(midx + step * 2, midy + step * 2);
    vertices.append(midx + step * 2, midy);
    vertices.append(midx + step, midy - step);
    vertices.append(midx - step, midy - step * 2);

    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::Normal, normals);
    painter->draw(QGL::TriangleFan, 5);

    // Overpaint some lines to show the triangle boundaries.
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(QColor(202, 170, 0));
    static ushort const indices[] = {0, 2, 0, 3};
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->draw(QGL::Lines, indices, 4);

    drawText(painter, rect, tr("Triangle fan"));
}

void ShapesWidget::paintCube(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    QGLSubsurface surface(painter->currentSurface(), rect);
    painter->pushSurface(&surface);

    painter->setCamera(&camera);
    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);

    cube->draw(painter);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();

    painter->popSurface();

    drawText(painter, rect, tr("Cube"));
}

void ShapesWidget::paintTeapot(QGLPainter *painter, const QRect& rect)
{
    painter->setStandardEffect(QGL::LitMaterial);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    QGLSubsurface surface(painter->currentSurface(), rect);
    painter->pushSurface(&surface);

    painter->setCamera(&camera);
    // Need a one-sided lighting model for the teapot.
    painter->setLightModel(&oneSidedModel);
    teapot->draw(painter);

    painter->setLightModel(&twoSidedModel);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();

    painter->popSurface();

    drawText(painter, rect, tr("Teapot"));
}

// Draw text centered on the bottom of the "posn" rectangle.
void ShapesWidget::drawText
        (QGLPainter *painter, const QRect& posn, const QString& str)
{
    QFont f = QGuiApplication::font();
    QFontMetrics metrics(f);
    QRect rect = metrics.boundingRect(str);
    rect.adjust(0, 0, 1, 1);

    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter p2(&image);
    p2.setFont(f);
    p2.setPen(Qt::white);
    p2.drawText(-rect.x(), metrics.ascent(), str);
    p2.end();

    QGLTexture2D texture;
    texture.setImage(image);

    int x = posn.x() + (posn.width() - rect.width()) / 2;
    int y = posn.y() + posn.height() - metrics.ascent() - metrics.descent();
    y -= 10;

    QVector2DArray vertices;
    vertices.append(x + rect.x(), y + metrics.ascent());
    vertices.append(x + rect.x(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y + metrics.ascent());

    QVector2DArray texCoord;
    texCoord.append(0.0f, 0.0f);
    texCoord.append(0.0f, 1.0f);
    texCoord.append(1.0f, 1.0f);
    texCoord.append(1.0f, 0.0f);

    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    texture.bind();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
    painter->draw(QGL::TriangleFan, 4);
    painter->setStandardEffect(QGL::FlatColor);
    glBindTexture(GL_TEXTURE_2D, 0);
    texture.cleanupResources();
}

void ShapesWidget::ensureContext()
{
    if (!context)
    {
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        context = new QOpenGLContext();
        context->setFormat(format);
#ifndef QT_NO_DEBUG_STREAM
        QSurfaceFormat oldFormat = format;
#endif
        context->create();
        // TODO: is it possible that the platform will downgrade the actual
        // format, or will it just fail if it can't deliver the actual format
        format = context->format();
#ifndef QT_NO_DEBUG_STREAM
        if (oldFormat.swapBehavior() != format.swapBehavior())
            qWarning() << "Could not create context for swap behavior"
                       << oldFormat.swapBehavior();
#endif
        context->makeCurrent(this);
    }
    else
    {
        context->makeCurrent(this);
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    ShapesWidget view;

    QStringList args = QCoreApplication::arguments();
    int w_pos = args.indexOf("-width");
    int h_pos = args.indexOf("-height");
    if (w_pos >= 0 && h_pos >= 0)
    {
        bool ok = true;
        int w = args.at(w_pos + 1).toInt(&ok);
        if (!ok)
        {
            qWarning() << "Could not parse width argument:" << args;
            return 1;
        }
        int h = args.at(h_pos + 1).toInt(&ok);
        if (!ok)
        {
            qWarning() << "Could not parse height argument:" << args;
            return 1;
        }
        view.resize(w, h);
    }
    else
    {
        view.resize(800, 600);
    }
    view.show();

    return app.exec();
}



#include "shapes.moc"
