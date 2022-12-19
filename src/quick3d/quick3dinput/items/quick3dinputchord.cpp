// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3dinputchord_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DInputChord::Quick3DInputChord(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractActionInput> Quick3DInputChord::qmlActionInputs()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAbstractActionInput;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *input) {
        Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
        action->parentChord()->addChord(input);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
        return action->parentChord()->chords().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
        return action->parentChord()->chords().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
        const auto chords = action->parentChord()->chords();
        for (QAbstractActionInput *input : chords)
            action->parentChord()->removeChord(input);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3dinputchord_p.cpp"
