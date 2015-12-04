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

#include "quick3daxis_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DAxis::Quick3DAxis(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAxisInput> Quick3DAxis::qmlAxisInputs()
{
    return QQmlListProperty<QAxisInput>(this, 0,
                                        &Quick3DAxis::appendAxisInput,
                                        &Quick3DAxis::axesInputCount,
                                        &Quick3DAxis::axisInputAt,
                                        &Quick3DAxis::clearAxisInputs);
}

void Quick3DAxis::appendAxisInput(QQmlListProperty<QAxisInput> *list, QAxisInput *input)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    axis->parentAxis()->addInput(input);
}

QAxisInput *Quick3DAxis::axisInputAt(QQmlListProperty<QAxisInput> *list, int index)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    return axis->parentAxis()->inputs().at(index);
}

int Quick3DAxis::axesInputCount(QQmlListProperty<QAxisInput> *list)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    return axis->parentAxis()->inputs().count();
}

void Quick3DAxis::clearAxisInputs(QQmlListProperty<QAxisInput> *list)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    Q_FOREACH (QAxisInput *input, axis->parentAxis()->inputs())
        axis->parentAxis()->removeInput(input);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
