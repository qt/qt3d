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

#include <Qt3DQuickInput/private/quick3daxis_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DAxis::Quick3DAxis(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractAxisInput> Quick3DAxis::qmlAxisInputs()
{
    return QQmlListProperty<QAbstractAxisInput>(this, 0,
                                        &Quick3DAxis::appendAxisInput,
                                        &Quick3DAxis::axesInputCount,
                                        &Quick3DAxis::axisInputAt,
                                        &Quick3DAxis::clearAxisInputs);
}

void Quick3DAxis::appendAxisInput(QQmlListProperty<QAbstractAxisInput> *list, QAbstractAxisInput *input)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    axis->parentAxis()->addInput(input);
}

QAbstractAxisInput *Quick3DAxis::axisInputAt(QQmlListProperty<QAbstractAxisInput> *list, int index)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    return axis->parentAxis()->inputs().at(index);
}

int Quick3DAxis::axesInputCount(QQmlListProperty<QAbstractAxisInput> *list)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    return axis->parentAxis()->inputs().count();
}

void Quick3DAxis::clearAxisInputs(QQmlListProperty<QAbstractAxisInput> *list)
{
    Quick3DAxis *axis = qobject_cast<Quick3DAxis *>(list->object);
    const auto inputs = axis->parentAxis()->inputs();
    for (QAbstractAxisInput *input : inputs)
        axis->parentAxis()->removeInput(input);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3daxis_p.cpp"
