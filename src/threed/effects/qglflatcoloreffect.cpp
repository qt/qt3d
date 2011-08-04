/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglflatcoloreffect_p.h"
#include "qglabstracteffect_p.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFlatColorEffect
    \since 4.8
    \brief The QGLFlatColorEffect class provides a standard effect that draws fragments with a flat unlit color.
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal
*/

/*!
    \class QGLPerVertexColorEffect
    \since 4.8
    \brief The QGLPerVertexColorEffect class provides a standard effect that draws fragments with a per-vertex unlit color.
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal
*/

class QGLFlatColorEffectPrivate
{
public:
    QGLFlatColorEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , colorUniform(-1)
        , isFixedFunction(false)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
    int colorUniform;
    bool isFixedFunction;
};

/*!
    Constructs a new flat color effect.
*/
QGLFlatColorEffect::QGLFlatColorEffect()
    : d_ptr(new QGLFlatColorEffectPrivate)
{
}

/*!
    Destroys this flat color effect.
*/
QGLFlatColorEffect::~QGLFlatColorEffect()
{
}

/*!
    \reimp
*/
bool QGLFlatColorEffect::supportsPicking() const
{
    return true;
}

/*!
    \reimp
*/
void QGLFlatColorEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    if (flag)
        glEnableClientState(GL_VERTEX_ARRAY);
    else
        glDisableClientState(GL_VERTEX_ARRAY);
#else
    Q_UNUSED(painter);
    Q_D(QGLFlatColorEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag)
            glEnableClientState(GL_VERTEX_ARRAY);
        else
            glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }
#endif
    static char const flatColorVertexShader[] =
        "attribute highp vec4 vertex;\n"
        "uniform highp mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    static char const flatColorFragmentShader[] =
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";

    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.color.flat"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatColorFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        if (!program->link()) {
            qWarning("QGLFlatColorEffect::setActive(): could not link shader program");
            delete program;
            return;
        }
        painter->setCachedProgram(QLatin1String("qt.color.flat"), program);
        d->program = program;
        d->colorUniform = program->uniformLocation("color");
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
    } else if (flag) {
        d->colorUniform = program->uniformLocation("color");
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction
        (updates & (QGLPainter::UpdateColor |
                    QGLPainter::UpdateMatrices));
#else
    Q_D(QGLFlatColorEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        painter->updateFixedFunction
            (updates & (QGLPainter::UpdateColor |
                        QGLPainter::UpdateMatrices));
        return;
    }
#endif
    if ((updates & QGLPainter::UpdateColor) != 0) {
        if (painter->isPicking())
            d->program->setUniformValue(d->colorUniform, painter->pickColor());
        else
            d->program->setUniformValue(d->colorUniform, painter->color());
    }
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        QMatrix4x4 proj = painter->projectionMatrix();
        QMatrix4x4 mv = painter->modelViewMatrix();
        d->program->setUniformValue(d->matrixUniform, proj * mv);
    }
#endif
}

class QGLPerVertexColorEffectPrivate
{
public:
    QGLPerVertexColorEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , isFixedFunction(false)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
    bool isFixedFunction;
};

/*!
    Constructs a new per-vertex color effect.
*/
QGLPerVertexColorEffect::QGLPerVertexColorEffect()
    : d_ptr(new QGLPerVertexColorEffectPrivate)
{
}

/*!
    Destroys this per-vertex color effect.
*/
QGLPerVertexColorEffect::~QGLPerVertexColorEffect()
{
}

/*!
    \reimp
*/
void QGLPerVertexColorEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
#else
    Q_UNUSED(painter);
    Q_D(QGLPerVertexColorEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag) {
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
        } else {
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
        }
        return;
    }
#endif
    static char const pvColorVertexShader[] =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 color;\n"
        "uniform highp mat4 matrix;\n"
        "varying mediump vec4 qColor;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    qColor = color;\n"
        "}\n";

    static char const pvColorFragmentShader[] =
        "varying mediump vec4 qColor;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = qColor;\n"
        "}\n";

    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.color.pervertex"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, pvColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, pvColorFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("color", QGL::Color);
        if (!program->link()) {
            qWarning("QGLPerVertexColorEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram(QLatin1String("qt.color.pervertex"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Color);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Color);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::Color);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLPerVertexColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction(updates & QGLPainter::UpdateMatrices);
#else
    Q_UNUSED(painter);
    Q_D(QGLPerVertexColorEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        painter->updateFixedFunction(updates & QGLPainter::UpdateMatrices);
        return;
    }
#endif
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue
            (d->matrixUniform, painter->combinedMatrix());
    }
#endif
}

QT_END_NAMESPACE
