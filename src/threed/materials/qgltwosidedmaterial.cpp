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

#include "qgltwosidedmaterial.h"
#include "qglpainter.h"
#include "qgllightmodel.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLTwoSidedMaterial
    \since 4.8
    \brief The QGLTwoSidedMaterial class implemented two-sided materials for 3D rendering.
    \ingroup qt3d
    \ingroup qt3d::materials

    Two-sided materials consist of a front() material and a back()
    material.  The specific material rendered is determined by the
    direction faced by a fragment when it is rendered.  Fragments
    that point towards the viewer are rendered with front(),
    and fragments that point away from the viewer are rendered
    with back().  In both cases, any textures that are used to
    render the material are taken from front().

    If front() and back() are the same, then the same material
    will be used on both sides of the fragment.  However, this
    is not exactly the same as using a one-sided QGLMaterial in
    place of the two-sided material.  One-sided materials will
    render the back of the fragment as black because the normal
    is always pointing away from the viewer.  Two-sided materials
    reverse the back-facing normal so that back() is lit as
    though it was on a front-facing face.

    \sa QGLMaterial
*/

class QGLTwoSidedMaterialPrivate
{
public:
    QGLTwoSidedMaterialPrivate() : front(0), back(0), defaultMaterial(0) {}

    QGLMaterial *front;
    QGLMaterial *back;
    QGLMaterial *defaultMaterial;
};

/*!
    Constructs a two-sided material object and attaches it to \a parent.

    \sa setFront(), setBack()
*/
QGLTwoSidedMaterial::QGLTwoSidedMaterial(QObject *parent)
    : QGLAbstractMaterial(parent)
    , d_ptr(new QGLTwoSidedMaterialPrivate)
{
}

/*!
    Destroys this two-sided material object.
*/
QGLTwoSidedMaterial::~QGLTwoSidedMaterial()
{
}

/*!
    \property QGLTwoSidedMaterial::front
    \brief the material for the front side of the object's fragments.

    The default value is null, which will result in a default QGLMaterial
    object being used when bind() is called.

    \sa back(), frontChanged(), materialChanged()
*/

QGLMaterial *QGLTwoSidedMaterial::front() const
{
    Q_D(const QGLTwoSidedMaterial);
    return d->front;
}

void QGLTwoSidedMaterial::setFront(QGLMaterial *material)
{
    Q_D(QGLTwoSidedMaterial);
    if (d->front != material) {
        if (d->front && d->front != d->back) {
            disconnect(d->front, SIGNAL(materialChanged()),
                       this, SIGNAL(materialChanged()));
        }
        d->front = material;
        if (d->front && d->front != d->back) {
            connect(d->front, SIGNAL(materialChanged()),
                    this, SIGNAL(materialChanged()));
        }
        emit frontChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLTwoSidedMaterial::back
    \brief the material for the back side of the object's fragments.

    The default value is null, which indicates that front() should
    be used on both the front and back sides of the object's fragments.

    Textures are taken from the front() material.  Any textures that
    appear in the back() material are ignored.  Only the material
    lighting parameters from back() will be used.

    \sa front(), backChanged(), materialChanged()
*/

QGLMaterial *QGLTwoSidedMaterial::back() const
{
    Q_D(const QGLTwoSidedMaterial);
    return d->back;
}

void QGLTwoSidedMaterial::setBack(QGLMaterial *material)
{
    Q_D(QGLTwoSidedMaterial);
    if (d->back != material) {
        if (d->back && d->back != d->front) {
            disconnect(d->back, SIGNAL(materialChanged()),
                       this, SIGNAL(materialChanged()));
        }
        d->back = material;
        if (d->back && d->back != d->front) {
            connect(d->back, SIGNAL(materialChanged()),
                    this, SIGNAL(materialChanged()));
        }
        emit backChanged();
        emit materialChanged();
    }
}

/*!
    \reimp
*/
bool QGLTwoSidedMaterial::isTransparent() const
{
    Q_D(const QGLTwoSidedMaterial);
    if (d->front && d->front->isTransparent())
        return true;
    return d->back && d->back->isTransparent();
}

/*!
    \reimp
*/
void QGLTwoSidedMaterial::bind(QGLPainter *painter)
{
    Q_D(QGLTwoSidedMaterial);
    QGLMaterial *front = d->front;
    if (!front) {
        // We need to have something for the front material.
        front = d->defaultMaterial;
        if (!front) {
            d->defaultMaterial = new QGLMaterial(this);
            front = d->defaultMaterial;
        }
    }
    const_cast<QGLLightModel *>(painter->lightModel())
        ->setModel(QGLLightModel::TwoSided); // FIXME
    if (d->back && d->back != front) {
        painter->setFaceMaterial(QGL::FrontFaces, front);
        painter->setFaceMaterial(QGL::BackFaces, d->back);
    } else {
        painter->setFaceMaterial(QGL::AllFaces, front);
    }
    front->bindTextures(painter);
}

/*!
    \reimp
*/
void QGLTwoSidedMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
{
    Q_D(const QGLTwoSidedMaterial);
    if (d->front)
        d->front->release(painter, next);
    else if (d->defaultMaterial)
        d->defaultMaterial->release(painter, next);
}

/*!
    \reimp
*/
void QGLTwoSidedMaterial::prepareToDraw
    (QGLPainter *painter, const QGLAttributeSet &attributes)
{
    Q_D(QGLTwoSidedMaterial);
    QGLMaterial *front = d->front;
    if (!front)
        front = d->defaultMaterial;
    front->bindEffect(painter, attributes, true);
}

/*!
    \fn void QGLTwoSidedMaterial::frontChanged()

    Signal that is emitted when the front() material pointer changes.

    This signal is not emitted when a property on front() changes.
    Use materialChanged() for that purpose instead.

    \sa front(), backChanged()
*/

/*!
    \fn void QGLTwoSidedMaterial::backChanged()

    Signal that is emitted when the back() material pointer changes.

    This signal is not emitted when a property on back() changes.
    Use materialChanged() for that purpose instead.

    \sa back(), frontChanged()
*/

QT_END_NAMESPACE
