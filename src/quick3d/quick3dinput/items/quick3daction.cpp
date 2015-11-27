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

#include "quick3daction_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DAction::Quick3DAction(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QActionInput> Quick3DAction::qmlActionInputs()
{
    return QQmlListProperty<QActionInput>(this, 0,
                                        &Quick3DAction::appendActionInput,
                                        &Quick3DAction::actionInputCount,
                                        &Quick3DAction::actionInputAt,
                                        &Quick3DAction::clearActionInputs);
}

void Quick3DAction::appendActionInput(QQmlListProperty<QActionInput> *list, QActionInput *input)
{
    Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
    action->parentAction()->addInput(input);
}

QActionInput *Quick3DAction::actionInputAt(QQmlListProperty<QActionInput> *list, int index)
{
    Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
    return action->parentAction()->inputs().at(index);
}

int Quick3DAction::actionInputCount(QQmlListProperty<QActionInput> *list)
{
    Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
    return action->parentAction()->inputs().count();
}

void Quick3DAction::clearActionInputs(QQmlListProperty<QActionInput> *list)
{
    Quick3DAction *action = qobject_cast<Quick3DAction *>(list->object);
    Q_FOREACH (QActionInput *input, action->parentAction()->inputs())
        action->parentAction()->removeInput(input);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
