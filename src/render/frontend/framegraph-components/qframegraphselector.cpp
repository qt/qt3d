/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qframegraphselector.h"
#include "qframegraphselector_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QFrameGraphSelectorPrivate
    \internal
*/
QFrameGraphSelectorPrivate::QFrameGraphSelectorPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
    \class Qt3D::QFrameGraphSelector

    \brief Qt3D::QFrameGraphSelector provide a way to select a one or more subtree of
    the FrameGraph at runtime

     The selection is made by executing Qt3D::QFrameGraphSelectorFunctor that will
     return a vector containing the indices of the FrameGraph subtree to be used
     for the current frame.
 */

/*!
    Constructs a new Qt3D::QFrameGraphSelector instance using \a parent as parent.
 */
QFrameGraphSelector::QFrameGraphSelector(QNode *parent)
    : QFrameGraphNode(*new QFrameGraphSelectorPrivate, parent)
{
}

QFrameGraphSelector::~QFrameGraphSelector()
{
    QNode::cleanup();
}

/*!
    \return the functor to be used for selecting FrameGraph subtrees.
 */
QFrameGraphSelectorFunctorPtr QFrameGraphSelector::selectionFunctor() const
{
    Q_D(const QFrameGraphSelector);
    return d->m_selectionFunctor;
}

/*!
     Sets the functor \a functor to be used for selecting FrameGrapg subtrees at
     runtime.
 */
void QFrameGraphSelector::setSelectionFunctor(QFrameGraphSelectorFunctorPtr functor)
{
    Q_D(QFrameGraphSelector);
    d->m_selectionFunctor = functor;
}

void QFrameGraphSelector::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
}

} // Qt3D

QT_END_NAMESPACE
