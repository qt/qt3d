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

#include "qgraphicstransform3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class QQuickQGraphicsTransform3D
    \brief The QQuickQGraphicsTransform3D class is an abstract base class for building 3D transformations.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    QQuickQGraphicsTransform3D lets you create and control advanced transformations
    that can be configured independently using specialized properties.
    Scene nodes have an associated list of transforms, which are applied
    in order, one at a time, to the modelview matrix.  Transformations are
    computed in true 3D space using QMatrix4x4.

    QQuickQGraphicsTransform3D is particularly useful for animations. Whereas
    QGLPainter::modelViewMatrix() lets you assign any transform directly,
    there is no direct way to interpolate between two different
    transformations (e.g., when transitioning between two states, each for
    which the item has a different arbitrary transform assigned). Using
    QQuickQGraphicsTransform3D you can interpolate the property values of each
    independent transformation. The resulting operation is then combined into a
    single transform which is applied to the modelview matrix during drawing.

    If you want to create your own configurable transformation, you can create
    a subclass of QQuickQGraphicsTransform3D (or any or the existing subclasses), and
    reimplement the pure virtual applyTo() function, which takes a pointer to a
    QMatrix4x4. Each operation you would like to apply should be exposed as
    properties (e.g., customTransform->setVerticalShear(2.5)). Inside you
    reimplementation of applyTo(), you can modify the provided transform
    respectively.

    \sa QGraphicsScale3D, QGraphicsRotation3D, QGraphicsTranslation3D
    \sa QGraphicsBillboardTransform
*/

/*!
    \fn QQuickQGraphicsTransform3D::QQuickQGraphicsTransform3D(QObject *parent)

    Constructs a 3D transformation and attaches it to \a parent.
*/

/*!
    \fn QQuickQGraphicsTransform3D::~QQuickQGraphicsTransform3D()

    Destroys this 3D transformation.
*/

/*!
    \fn void QQuickQGraphicsTransform3D::applyTo(QMatrix4x4 *matrix) const

    Applies the effect of this transformation to the specified
    modelview \a matrix.
*/

/*!
    \fn QQuickQGraphicsTransform3D *QQuickQGraphicsTransform3D::clone(QObject *parent) const

    Clones a copy of this transformation and attaches the clone to \a parent.
*/

/*!
    \fn void QQuickQGraphicsTransform3D::transformChanged()

    Signal that is emitted whenever any of the transformation's
    parameters are changed.
*/

QT_END_NAMESPACE
