// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3daction_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DAction::Quick3DAction(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractActionInput> Quick3DAction::qmlActionInputs()
{
    using qt_size_type = qsizetype;
    auto appendFunction = [](QQmlListProperty<QAbstractActionInput> *list, QAbstractActionInput *input) {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        action->parentAction()->addInput(input);
    };
    auto countFunction = [](QQmlListProperty<QAbstractActionInput> *list) -> qt_size_type {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        return action->parentAction()->inputs().size();
    };
    auto atFunction = [](QQmlListProperty<QAbstractActionInput> *list, qt_size_type index) -> QAbstractActionInput * {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        return action->parentAction()->inputs().at(index);
    };
    auto clearFunction = [](QQmlListProperty<QAbstractActionInput> *list) {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        const auto inputs = action->parentAction()->inputs();
        for (QAbstractActionInput *input : inputs)
            action->parentAction()->removeInput(input);
    };

    return QQmlListProperty<QAbstractActionInput>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3daction_p.cpp"
