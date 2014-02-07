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

#include <QPropertyAnimation>

#include "basketview.h"
#include "basket_data.h"
#include "qglbuilder.h"
#include "qglscenenode.h"

BasketView::BasketView(QWindow *parent)
    : QGLView(parent)
    , m_angle(0)
{
    setTitle(tr("Basket"));

//! [1]
    QGLBuilder builder;
    builder << BasketPatches();
    basket = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial;
    QUrl url;
    url.setPath(QLatin1String(":/basket.jpg"));
    url.setScheme(QLatin1String("file"));
    mat->setTextureUrl(url);
    basket->setMaterial(mat);
    basket->setEffect(QGL::LitModulateTexture2D);
//! [1]

//! [2]
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(this, "angle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(2000);
    animation->setLoopCount(-1);
    animation->start();
//! [2]

    camera()->setEye(QVector3D(0, 4, 10));
}

BasketView::~BasketView()
{
    const QGLMaterialCollection* pPalette = basket->palette().data();
    for (int i=0; i<pPalette->size(); ++i) {
        pPalette->material(i)->texture()->cleanupResources();
    }
    delete basket;
}

//! [3]
void BasketView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(angle(), 0, 1, 0);
    painter->modelViewMatrix().scale(1.5f);
    basket->draw(painter);
}
//! [3]
