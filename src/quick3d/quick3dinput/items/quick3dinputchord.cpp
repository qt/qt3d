/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
    return QQmlListProperty<QAbstractActionInput>(this, 0,
                                        &Quick3DInputChord::appendActionInput,
                                        &Quick3DInputChord::actionInputCount,
                                        &Quick3DInputChord::actionInputAt,
                                        &Quick3DInputChord::clearActionInputs);
}

void Quick3DInputChord::appendActionInput(QQmlListProperty<QAbstractActionInput> *list, QAbstractActionInput *input)
{
    Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
    action->parentChord()->addChord(input);
}

QAbstractActionInput *Quick3DInputChord::actionInputAt(QQmlListProperty<QAbstractActionInput> *list, int index)
{
    Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
    return action->parentChord()->chords().at(index);
}

int Quick3DInputChord::actionInputCount(QQmlListProperty<QAbstractActionInput> *list)
{
    Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
    return action->parentChord()->chords().count();
}

void Quick3DInputChord::clearActionInputs(QQmlListProperty<QAbstractActionInput> *list)
{
    Quick3DInputChord *action = qobject_cast<Quick3DInputChord *>(list->object);
    const auto chords = action->parentChord()->chords();
    for (QAbstractActionInput *input : chords)
        action->parentChord()->removeChord(input);
}

} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
