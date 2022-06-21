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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    auto appendFunction = [](QQmlListProperty<QAbstractActionInput> *list, QAbstractActionInput *input) {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        action->parentAction()->addInput(input);
    };
    auto countFunction = [](QQmlListProperty<QAbstractActionInput> *list) -> qt_size_type {
        Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
        return action->parentAction()->inputs().count();
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
