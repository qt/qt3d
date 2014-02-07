/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tank.h"

#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QImage>
#include <QPainter>
#include <QSize>

#ifndef QT_NO_DEBUG
#include <QThread>
#include <QCoreApplication>
#endif

#include "qglbuilder.h"
#include "qglcylinder.h"
#include "qglmaterial.h"
#include "qgltexture2d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicsrotation3d.h"

static inline int rval()
{
    // return an int 64 <--> 192 inclusive
    return (float(qrand()) / float(RAND_MAX)) * 128 + 64;
}

static QGLMaterial *qCreateFluid()
{
    QImage image(QSize(128,128), QImage::Format_ARGB32);
    QRgb col = qRgba(rval(), rval(), rval(), 196);
    image.fill(col);
    QPainter painter(&image);
    QLinearGradient linearGrad(QPointF(56, 56), QPointF(72, 72));
    linearGrad.setColorAt(0, col);
    linearGrad.setColorAt(1, QColor(col).lighter());
    linearGrad.setSpread(QGradient::ReflectSpread);
    painter.fillRect(image.rect(), linearGrad);
    painter.end();
    QGLMaterial *mat = new QGLMaterial;
    QColor white(Qt::white);
    white.setAlpha(128);
    mat->setAmbientColor(white);
    mat->setDiffuseColor(white);
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(image);
    mat->setTexture(tex);
    return mat;
}

Tank::Tank(QObject *parent)
    : QGLSceneNode(parent)
    , m_texture(0)
{
    QSequentialAnimationGroup *seq = new QSequentialAnimationGroup(this);
    QGraphicsScale3D *scale = new QGraphicsScale3D(this);
    addTransform(scale);
    QPropertyAnimation *anim = new QPropertyAnimation(scale, "scale");
    anim->setDuration(10000);
    anim->setStartValue(QVector3D(1.0f, 0.1f, 1.0f));
    anim->setEndValue(QVector3D(1.0f, 1.2f, 1.0f));
    anim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    seq->addAnimation(anim);
    seq->addPause(2000);
    anim = new QPropertyAnimation(scale, "scale");
    anim->setDuration(10000);
    anim->setStartValue(QVector3D(1.0f, 1.2f, 1.0f));
    anim->setEndValue(QVector3D(1.0f, 0.1f, 1.0f));
    anim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    seq->addAnimation(anim);
    seq->setLoopCount(-1);
    seq->start();

    addNode(tankObject());

    QGLMaterial *mat = qCreateFluid();
    m_texture = mat->texture();
    setMaterial(mat);
}

Tank::~Tank()
{
    if (m_texture)
        m_texture->cleanupResources();
}

QGLSceneNode *Tank::tankObject()
{
#ifndef QT_NO_DEBUG
    // NOT THREADSAFE, NOT RE-ENTRANT
    Q_ASSERT(QThread::currentThread() == qApp->thread());
#endif
    static QGLSceneNode *theTank = 0;
    if (!theTank)
    {
        QGLBuilder build;
        build << QGLCylinder(1.0f, 1.0f, 1.0f, 32);
        theTank = build.finalizedSceneNode();
        theTank->setObjectName("Tank Component");
        QGraphicsRotation3D *rot = new QGraphicsRotation3D(theTank);
        rot->setAxis(QVector3D(1, 0, 0));
        rot->setAngle(-90.0f);
        theTank->addTransform(rot);
        theTank->setY(-theTank->boundingBox().minimum().y());
    }
    return theTank;
}
