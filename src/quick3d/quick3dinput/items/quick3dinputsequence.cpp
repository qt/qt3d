// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3dinputsequence_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DInputSequence::Quick3DInputSequence(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractActionInput> Quick3DInputSequence::qmlActionInputs()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAbstractActionInput;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *input) {
        Quick3DInputSequence *action = qobject_cast<Quick3DInputSequence *>(list->object);
        action->parentSequence()->addSequence(input);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DInputSequence *action = qobject_cast<Quick3DInputSequence *>(list->object);
        return action->parentSequence()->sequences().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DInputSequence *action = qobject_cast<Quick3DInputSequence *>(list->object);
        return action->parentSequence()->sequences().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DInputSequence *action = qobject_cast<Quick3DInputSequence *>(list->object);
        const auto sequences = action->parentSequence()->sequences();
        for (QAbstractActionInput *input : sequences)
            action->parentSequence()->removeSequence(input);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3dinputsequence_p.cpp"
