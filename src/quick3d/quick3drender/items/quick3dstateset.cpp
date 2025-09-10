// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = QRenderState;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *state) {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        stateSet->parentStateSet()->addRenderState(state);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DStateSet *stateSet = qobject_cast<Quick3DStateSet *>(list->object);
        return stateSet->parentStateSet()->renderStates().size();
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

