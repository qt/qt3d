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

#include <QGuiApplication>
#include <QTimer>
#include <QMouseEvent>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>

#include "qglpainter.h"
#include "qglabstracteffect.h"
#include "qgltexture2d.h"
#include "qglshaderprogrameffect.h"

#include "pageflipmath_p.h"

class PageFlipGradientEffect;

class PageFlipView : public QWindow
{
    Q_OBJECT
public:
    PageFlipView(QWindow *parent = 0);
    ~PageFlipView();

    void setBlend(bool value) { blend = value; }
    void setVertical(bool value) { vertical = value; }

public Q_SLOTS:
    void update();

protected:
    void resizeGL(int width, int height);
    void initializeGL();
    void paintGL();

    void exposeEvent(QExposeEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void animate();

private:
    void ensureContext();
    void setAlphaValue(QGLPainter *painter, GLfloat value);

    bool blend;
    bool vertical;

    float posn;     // Position within the animation - 0...1
    QSize pageSize; // Size of a page within the window.

    QRect pageRect1;
    QRect pageRect2;

    QColor colors[4];
    int colorIndex;

    QGLTexture2D textures[4];

    QGLTexture2D gradientTexture;

    PageFlipMath pageFlipMath;

    PageFlipGradientEffect *effect;

    QOpenGLContext *context;
    bool initialised;
    QSurfaceFormat format;
    bool updateQueued;
};

class PageFlipGradientEffect : public QGLShaderProgramEffect
{
public:
    PageFlipGradientEffect();
    ~PageFlipGradientEffect();

    void setAlphaValue(GLfloat value);
};

PageFlipView::PageFlipView(QWindow *parent)
    : QWindow(parent)
    , context(0)
    , initialised(false)
    , updateQueued(false)
{
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(24);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);

    posn = 0.0f;
    blend = false;
    vertical = false;

    colors[0] = QColor(0, 192, 192, 255);
    colors[1] = QColor(192, 0, 0, 255);
    colors[2] = QColor(192, 192, 0, 255);
    colors[3] = QColor(128, 128, 0, 255);
    colorIndex = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(40);

    effect = new PageFlipGradientEffect();
}

PageFlipView::~PageFlipView()
{
    textures[0].cleanupResources();
    textures[1].cleanupResources();
    textures[2].cleanupResources();
    textures[3].cleanupResources();
    gradientTexture.cleanupResources();
    delete effect;
}

void PageFlipView::exposeEvent(QExposeEvent *e)
{
    Q_UNUSED(e);

    updateQueued = false;
    ensureContext();

    QRect rect = geometry();
    resizeGL(rect.width(), rect.height());

    if (!initialised)
        initializeGL();

    paintGL();

    context->swapBuffers(this);
}

void PageFlipView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void PageFlipView::initializeGL()
{
    QGLPainter painter(this);

    //QSize size = rect().size();
    //int width = size.width() / 3;
    //int height = (int)(width * 1.414f);
    int width = 227;
    int height = 320;
    pageSize = QSize(width, height);

    textures[0].setImage(QImage(QLatin1String(":/qqpage1.png")));
    textures[1].setImage(QImage(QLatin1String(":/qqpage2.png")));
    textures[2].setImage(QImage(QLatin1String(":/qqpage3.png")));
    textures[3].setImage(QImage(QLatin1String(":/qqpage4.png")));

    gradientTexture.setImage(QImage(QLatin1String(":/gradient.png")));

    if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter.glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter.glBlendEquation(GL_FUNC_ADD);
    else if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    glEnable(GL_BLEND);

    if (vertical)
        pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomRight);
    else
        pageFlipMath.setStartCorner(PageFlipMath::BottomRight);

    initialised = true;
}

void PageFlipView::paintGL()
{
    QGLPainter painter(this);

    QRect rect = this->geometry();
    int midx = rect.width() / 2;
    int topy = (rect.height() - pageSize.height()) / 2;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 projm;
    projm.ortho(rect);
    painter.projectionMatrix() = projm;
    painter.modelViewMatrix().setToIdentity();

    if (vertical) {
        pageRect2 = QRect(QPoint(midx - pageSize.width() / 2, topy), pageSize);
        pageRect1 = QRect(QPoint(pageRect2.x() - pageSize.width(), topy), pageSize);
    } else {
        pageRect1 = QRect(QPoint(midx - pageSize.width(), topy), pageSize);
        pageRect2 = QRect(QPoint(midx, topy), pageSize);
    }
    pageFlipMath.setPageRect(pageRect2);
    pageFlipMath.setShowPageReverse(false);
    pageFlipMath.compute(posn);

    QGLAttributeValue positions
        (2, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray());
    QGLAttributeValue texCoords
        (2, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray() + 2);
    QGLAttributeValue gradientCoords
        (1, GL_FLOAT, pageFlipMath.stride(), pageFlipMath.vertexArray() + 4);

    if (painter.isFixedFunction())
        painter.setStandardEffect(QGL::FlatReplaceTexture2D);
    else
        painter.setUserEffect(effect);
    painter.setColor(colors[colorIndex]);
    painter.glActiveTexture(GL_TEXTURE0);
    textures[colorIndex].bind();
    if (!painter.isFixedFunction()) {
        painter.glActiveTexture(GL_TEXTURE1);
        gradientTexture.bind();
    }
    painter.clearAttributes();
    painter.setVertexAttribute(QGL::Position, positions);
    painter.setVertexAttribute(QGL::TextureCoord0, texCoords);
    painter.setVertexAttribute(QGL::CustomVertex0, gradientCoords);
    setAlphaValue(&painter, 1.0f);
    painter.update();
    pageFlipMath.drawPage(0);

    painter.setColor(colors[(colorIndex + 1) % 4]);
    painter.glActiveTexture(GL_TEXTURE0);
    textures[(colorIndex + 1) % 4].bind();
    setAlphaValue(&painter, 1.0f);
    painter.update();
    pageFlipMath.drawPage(1);

    painter.setColor(colors[(colorIndex + 2) % 4]);
    if (!pageFlipMath.showPageReverse())
        textures[(colorIndex + 2) % 4].bind();
    if (blend)
        setAlphaValue(&painter, 0.75f);
    else
        setAlphaValue(&painter, 1.0f);
    painter.update();
    pageFlipMath.drawPage(2);

    painter.setColor(colors[(colorIndex + 3) % 4]);
    textures[(colorIndex + 3) % 4].bind();
    setAlphaValue(&painter, 1.0f);
    painter.update();
    pageFlipMath.drawPage(3);

    glBindTexture(GL_TEXTURE_2D, 0);
    painter.glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    painter.setStandardEffect(QGL::FlatColor);
    painter.clearAttributes();
    painter.setVertexAttribute(QGL::Position, positions);
    painter.setVertexAttribute(QGL::TextureCoord0, texCoords);
    painter.setVertexAttribute(QGL::CustomVertex0, gradientCoords);
    painter.setColor(QColor(0, 0, 0, 255));
    painter.update();
    pageFlipMath.drawOutline(2);
}

void PageFlipView::mousePressEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();
    bool changed = true;
    if (vertical) {
        if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::VerticalTopLeft);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomLeft);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::VerticalTopRight);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::VerticalBottomRight);
        else
            changed = false;
    } else {
        if (x >= pageRect1.x() && x < (pageRect1.x() + 20) &&
                y >= pageRect1.y() && y < (pageRect1.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopLeft);
        else if (x >= pageRect1.x() && x < (pageRect1.x() + 20) &&
                    y >= (pageRect1.bottom() - 20) && y <= pageRect1.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomLeft);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopLeftOnePage);
        else if (x >= pageRect2.x() && x < (pageRect2.x() + 20) &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomLeftOnePage);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= pageRect2.y() && y < (pageRect2.y() + 20))
            pageFlipMath.setStartCorner(PageFlipMath::TopRight);
        else if (x >= (pageRect2.right() - 20) && x <= pageRect2.right() &&
                    y >= (pageRect2.bottom() - 20) && y <= pageRect2.bottom())
            pageFlipMath.setStartCorner(PageFlipMath::BottomRight);
        else
            changed = false;
    }
    if (changed)
        posn = 0.0f;
    QWindow::mousePressEvent(e);
}

void PageFlipView::animate()
{
    posn += 0.04f;
    if (posn >= 1.0f) {
        posn = 0.0f;
        colorIndex = (colorIndex + 2) % 4;
    }
    update();
}

void PageFlipView::update()
{
    if (!updateQueued)
    {
        updateQueued = true;
        QGuiApplication::postEvent(this, new QExposeEvent(geometry()));
    }
}

void PageFlipView::setAlphaValue(QGLPainter *painter, GLfloat value)
{
    if (!painter->isFixedFunction())
        effect->setAlphaValue(value);
}

static char const gradientVertexShader[] =
    "attribute highp vec4 qt_Vertex;\n"
    "attribute highp vec4 qt_MultiTexCoord0;\n"
    "attribute highp float qt_Custom0;\n"
    "uniform mediump mat4 qt_ModelViewProjectionMatrix;\n"
    "varying highp vec4 qt_TexCoord0;\n"
    "varying highp float qGradCtrl;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;\n"
    "    qt_TexCoord0 = qt_MultiTexCoord0;\n"
    "    qGradCtrl = qt_Custom0;\n"
    "}\n";

static char const gradientFragmentShader[] =
    "uniform sampler2D qt_Texture0;\n"
    "uniform sampler2D qt_Texture1;\n"
    "uniform mediump float alphaValue;\n"
    "varying highp vec4 qt_TexCoord0;\n"
    "varying highp float qGradCtrl;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(qt_Texture0, qt_TexCoord0.st);\n"
    "    mediump vec4 gradcol = texture2D(qt_Texture1, vec2(qGradCtrl, qt_TexCoord0.t));\n"
    "    gl_FragColor = vec4((col * gradcol).xyz, alphaValue);\n"
    "}\n";

PageFlipGradientEffect::PageFlipGradientEffect()
{
    setVertexShader(gradientVertexShader);
    setFragmentShader(gradientFragmentShader);
}

PageFlipGradientEffect::~PageFlipGradientEffect()
{
}

void PageFlipGradientEffect::setAlphaValue(GLfloat value)
{
    program()->setUniformValue("alphaValue", value);
}

inline void PageFlipView::ensureContext()
{
    if (!context)
    {
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
        if (oldFormat != format)
            qWarning() << "Could not create context requested:\n"
                       << oldFormat << "\nactual format:\n" << format;
#endif
    }
    context->makeCurrent(this);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    PageFlipView view;

    QStringList args = QCoreApplication::arguments();
    if (args.contains(QLatin1String("-blend")))
        view.setBlend(true);
    if (args.contains(QLatin1String("-vertical")))
        view.setVertical(true);

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

#include "pageflip.moc"
