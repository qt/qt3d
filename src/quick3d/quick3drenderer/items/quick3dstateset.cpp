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

#include "quick3dstateset.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DStateSet::Quick3DStateSet(QObject *parent)
    : QObject(parent)
{
}

Quick3DStateSet::~Quick3DStateSet()
{
}

QQmlListProperty<QRenderState> Quick3DStateSet::renderStateList()
{
    return QQmlListProperty<Qt3D::QRenderState>(this, 0,
                                              &Quick3DStateSet::appendRenderState,
                                              &Quick3DStateSet::renderStateCount,
                                              &Quick3DStateSet::renderStateAt,
                                              &Quick3DStateSet::clearRenderStates);

}

void Quick3DStateSet::appendRenderState(QQmlListProperty<QRenderState> *list, QRenderState *state)
{
    Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
    stateSet->parentStateSet()->addRenderState(state);
}

QRenderState *Quick3DStateSet::renderStateAt(QQmlListProperty<QRenderState> *list, int index)
{
    Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
    return stateSet->parentStateSet()->renderStates().at(index);
}

int Quick3DStateSet::renderStateCount(QQmlListProperty<QRenderState> *list)
{
    Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
    return stateSet->parentStateSet()->renderStates().count();
}

void Quick3DStateSet::clearRenderStates(QQmlListProperty<QRenderState> *list)
{
    Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
    Q_FOREACH (QRenderState *s, stateSet->parentStateSet()->renderStates())
        stateSet->parentStateSet()->removeRenderState(s);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

