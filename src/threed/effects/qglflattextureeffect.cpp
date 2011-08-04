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

#include "qglflattextureeffect_p.h"
#include "qglabstracteffect_p.h"
#include "qglext_p.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFlatTextureEffect
    \since 4.8
    \brief The QGLFlatTextureEffect class provides a standard effect that draws fragments with a flat unlit texture.
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal
*/

/*!
    \class QGLFlatDecalTextureEffect
    \since 4.8
    \brief The QGLFlatDecalTextureEffect class provides a standard effect that decals fragments with a flat unlit texture.
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal
*/

class QGLFlatTextureEffectPrivate
{
public:
    QGLFlatTextureEffectPrivate()
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
    Constructs a new flat texture effect.
*/
QGLFlatTextureEffect::QGLFlatTextureEffect()
    : d_ptr(new QGLFlatTextureEffectPrivate)
{
}

/*!
    Destroys this flat texture effect.
*/
QGLFlatTextureEffect::~QGLFlatTextureEffect()
{
}

#if !defined(QGL_FIXED_FUNCTION_ONLY)

static char const flatTexVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec4 texcoord;\n"
    "uniform highp mat4 matrix;\n"
    "varying highp vec4 qt_TexCoord0;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    qt_TexCoord0 = texcoord;\n"
    "}\n";

static char const flatTexFragmentShader[] =
    "uniform sampler2D tex;\n"
    "varying highp vec4 qt_TexCoord0;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = texture2D(tex, qt_TexCoord0.st);\n"
    "}\n";

static char const flatDecalFragmentShader[] =
    "uniform sampler2D tex;\n"
    "uniform mediump vec4 color;\n"
    "varying highp vec4 qt_TexCoord0;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qt_TexCoord0.st);\n"
    "    gl_FragColor = vec4(clamp(color.rgb * (1.0 - col.a) + col.rgb, 0.0, 1.0), color.a);\n"
    "}\n";

#endif

/*!
    \reimp
*/
void QGLFlatTextureEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        qt_gl_ClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glEnable(GL_TEXTURE_2D);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        qt_gl_ClientActiveTexture(GL_TEXTURE0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
#else
    Q_UNUSED(painter);
    Q_D(QGLFlatTextureEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag) {
            glEnableClientState(GL_VERTEX_ARRAY);
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glEnable(GL_TEXTURE_2D);
        } else {
            glDisableClientState(GL_VERTEX_ARRAY);
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
        }
        return;
    }
#endif
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.texture.flat.replace"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatTexVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatTexFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        if (!program->link()) {
            qWarning("QGLFlatTextureEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram
            (QLatin1String("qt.texture.flat.replace"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::TextureCoord0);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatTextureEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction(updates & QGLPainter::UpdateMatrices);
#else
    Q_D(QGLFlatTextureEffect);
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

class QGLFlatDecalTextureEffectPrivate
{
public:
    QGLFlatDecalTextureEffectPrivate()
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
    Constructs a new flat decal texture effect.
*/
QGLFlatDecalTextureEffect::QGLFlatDecalTextureEffect()
    : d_ptr(new QGLFlatDecalTextureEffectPrivate)
{
}

/*!
    Destroys this flat decal texture effect.
*/
QGLFlatDecalTextureEffect::~QGLFlatDecalTextureEffect()
{
}

/*!
    \reimp
*/
void QGLFlatDecalTextureEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        qt_gl_ClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glEnable(GL_TEXTURE_2D);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        qt_gl_ClientActiveTexture(GL_TEXTURE0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
#else
    Q_UNUSED(painter);
    Q_D(QGLFlatDecalTextureEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag) {
            glEnableClientState(GL_VERTEX_ARRAY);
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glEnable(GL_TEXTURE_2D);
        } else {
            glDisableClientState(GL_VERTEX_ARRAY);
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
        }
    }
#endif
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.texture.flat.decal"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatTexVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatDecalFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        if (!program->link()) {
            qWarning("QGLFlatDecalTextureEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram
            (QLatin1String("qt.texture.flat.decal"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        d->colorUniform = program->uniformLocation("color");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        d->colorUniform = program->uniformLocation("color");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::TextureCoord0);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatDecalTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction
        (updates & (QGLPainter::UpdateColor |
                    QGLPainter::UpdateMatrices));
#else
    Q_D(QGLFlatDecalTextureEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        painter->updateFixedFunction
            (updates & (QGLPainter::UpdateColor |
                        QGLPainter::UpdateMatrices));
        return;
    }
#endif
    if ((updates & QGLPainter::UpdateColor) != 0)
        d->program->setUniformValue(d->colorUniform, painter->color());
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue
            (d->matrixUniform, painter->combinedMatrix());
    }
#endif
}

QT_END_NAMESPACE
