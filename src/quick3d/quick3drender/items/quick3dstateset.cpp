/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include <Qt3DQuickRender/private/quick3dstateset_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
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
    return QQmlListProperty<QRenderState>(this, 0,
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
    const auto states = stateSet->parentStateSet()->renderStates();
    for (QRenderState *s : states)
        stateSet->parentStateSet()->removeRenderState(s);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dstateset_p.cpp"

