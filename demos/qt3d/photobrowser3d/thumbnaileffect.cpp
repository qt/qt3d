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

#include "thumbnaileffect.h"

#include <QOpenGLShaderProgram>

class ThumbnailEffectPrivate
{
public:
    ThumbnailEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , thumbnailUniform(-1)
        , colorUniform(0)
        , pickingUniform(0)
        , thumbnail(false)
        , thumbnailUpdateRequired(false)
    {
    }
    ~ThumbnailEffectPrivate() { delete program; }

    QOpenGLShaderProgram *program;
    int matrixUniform;
    int thumbnailUniform;
    int colorUniform;
    int pickingUniform;
    // true if we render a thumbnail, false do the large size
    bool thumbnail;
    bool thumbnailUpdateRequired;
};

/*!
    Constructs a new flat texture effect.
*/
ThumbnailEffect::ThumbnailEffect()
    : d(new ThumbnailEffectPrivate)
{
}

/*!
    Destroys this flat texture effect.
*/
ThumbnailEffect::~ThumbnailEffect()
{
    delete d;
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> ThumbnailEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    fields += QGL::TextureCoord1;
    return fields;
}

/*!
    \reimp
*/
void ThumbnailEffect::setActive(QGLPainter *painter, bool flag)
{
    Q_UNUSED(painter);
    if (!d->program) {
        if (!flag)
            return;
        d->program = new QOpenGLShaderProgram();
        d->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/replace_texture.vsh");
        d->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/replace_texture.fsh");
        d->program->bindAttributeLocation("vertex", QGL::Position);
        d->program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        d->program->bindAttributeLocation("thumbcoord", QGL::TextureCoord1);
        if (!d->program->link()) {
            qWarning("ThumbnailEffect::setActive(): could not link shader d->program");
            delete d->program;
            d->program = 0;
            return;
        }
        d->matrixUniform = d->program->uniformLocation("matrix");
        d->thumbnailUniform = d->program->uniformLocation("thumb");
        d->colorUniform = d->program->uniformLocation("color");
        d->pickingUniform = d->program->uniformLocation("picking");
        d->program->bind();
        d->program->setUniformValue("texture", 0);
        d->program->enableAttributeArray(QGL::Position);
        d->program->enableAttributeArray(QGL::TextureCoord0);
        d->program->enableAttributeArray(QGL::TextureCoord1);
    } else if (flag) {
        d->matrixUniform = d->program->uniformLocation("matrix");
        d->program->bind();
        d->program->setUniformValue("texture", 0);
        d->program->enableAttributeArray(QGL::Position);
        d->program->enableAttributeArray(QGL::TextureCoord0);
        d->program->enableAttributeArray(QGL::TextureCoord1);
    } else {
        d->program->disableAttributeArray(QGL::Position);
        d->program->disableAttributeArray(QGL::TextureCoord0);
        d->program->disableAttributeArray(QGL::TextureCoord1);
        d->program->release();
    }
}

/*!
    \reimp
*/
void ThumbnailEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateMatrices) != 0)
    {
        d->program->setUniformValue(d->matrixUniform,
                                    painter->combinedMatrix());
    }
    if ((updates & QGLPainter::UpdateColor) != 0) {
        d->program->setUniformValue(d->pickingUniform, painter->isPicking());
        if (painter->isPicking())
            d->program->setUniformValue(d->colorUniform, painter->pickColor());
        else
            d->program->setUniformValue(d->colorUniform, Qt::green);
    }
    if (d->thumbnailUpdateRequired)
    {
        d->program->setUniformValue(d->thumbnailUniform, d->thumbnail);
        d->thumbnailUpdateRequired = false;
    }
}

bool ThumbnailEffect::supportsPicking() const
{
    return true;
}


void ThumbnailEffect::setThumbnail(bool enable)
{
    if (d->thumbnail != enable)
    {
        d->thumbnailUpdateRequired = true;
        d->thumbnail = enable;
    }
}

bool ThumbnailEffect::thumbnail() const
{
    return d->thumbnail;
}
