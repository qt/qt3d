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

#include "qglrenderordercomparator.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLRenderOrderComparator
    \brief The QGLRenderOrderComparator class compares QGLRenderOrder instances.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The QGLRenderOrderComparator class works with the QGLRenderSequencer and
    QGLRenderOrder classes to optimize the rendering order of scene nodes.

    This class is responsible for comparing QGLRenderOrder instances for
    the QGLRenderSequencer.

    Custom render orders may be created by sub-classing QGLRenderOrderComparator
    and reimplementing the following method:
    \list
        \i bool operator()(const QGLRenderOrder &lhs, const QGLRenderOrder &rhs)
    \endlist

    Then set an instance of your class onto QGLPainter:

    \code
    void MyView::initializeGL(QGLPainter *painter)
    {
        painter->renderSequencer()->setRenderOrderComparator(new MyRenderOrderComparator);
    }

    void MyView::paintGL(QGLPainter *painter)
    {
        // draw uses QGLRenderOrder sub-class instances from m_renderOrderFactory
        complexScene->draw(painter);
    }
    \endcode

    See the QGLRenderOrder class documentation for more details.

    \sa QGLRenderOrder
*/

/*!
    \fn QGLRenderOrderComparator::QGLRenderOrderComparator()
    Construct a new QGLRenderOrderComparator.
*/

/*!
    \fn QGLRenderOrderComparator::~QGLRenderOrderComparator()
    Destroys this QGLRenderOrderComparator, recovering any resources.
*/

/*!
    Returns true if the \a lhs render order is less than the \a rhs;
    otherwise returns false.

    Reimplement this function when creating custom render orders.
*/
bool QGLRenderOrderComparator::isLessThan(const QGLRenderOrder &lhs, const QGLRenderOrder &rhs)
{
    bool result = false;
    bool lhsHasEffect = lhs.effectiveHasEffect();
    bool rhsHasEffect = rhs.effectiveHasEffect();
    if (lhsHasEffect && rhsHasEffect)
    {
        QGLAbstractEffect *eff = lhs.effectiveUserEffect();
        if (eff)
            result = (eff < rhs.effectiveUserEffect());
        else
            result = !rhs.effectiveUserEffect() &&
                     (lhs.effectiveStandardEffect() < rhs.effectiveStandardEffect());
    }
    else
    {
        result = !lhsHasEffect;
    }
    if (!result)
    {
        result = (lhs.effectiveMaterial() < rhs.effectiveMaterial());
    }
    if (!result)
    {
        result = (lhs.effectiveBackMaterial() < rhs.effectiveBackMaterial());
    }
    return result;
}

/*!
    Returns true if the \a lhs render order is equal to the \a rhs;
    otherwise returns false.

    Reimplement this function when creating custom render orders.
*/
bool QGLRenderOrderComparator::isEqualTo(const QGLRenderOrder &lhs, const QGLRenderOrder &rhs)
{
    bool result = false;
    bool lhsHasEffect = lhs.effectiveHasEffect();
    bool rhsHasEffect = rhs.effectiveHasEffect();
    if (lhsHasEffect && rhsHasEffect)
    {
        QGLAbstractEffect *eff = lhs.effectiveUserEffect();
        if (eff)
            result = (eff == rhs.effectiveUserEffect());
        else
            result = (lhs.effectiveStandardEffect() == rhs.effectiveStandardEffect());
    }
    else
    {
        result = (lhsHasEffect == rhsHasEffect);
    }
    if (result)
    {
        result = (lhs.effectiveMaterial() == rhs.effectiveMaterial());
    }
    if (result)
    {
        result = (lhs.effectiveBackMaterial() == rhs.effectiveBackMaterial());
    }
    return result;
}

QT_END_NAMESPACE
