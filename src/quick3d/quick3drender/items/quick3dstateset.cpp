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
******************************************************************************/

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QRenderState;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *state) {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        stateSet->parentStateSet()->addRenderState(state);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        return stateSet->parentStateSet()->renderStates().count();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        return stateSet->parentStateSet()->renderStates().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        const auto states = stateSet->parentStateSet()->renderStates();
        for (QRenderState *s : states)
            stateSet->parentStateSet()->removeRenderState(s);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dstateset_p.cpp"

