/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#include "quick3dmorphtarget_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DMorphTarget::QQuick3DMorphTarget(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DRender::QAttribute> QQuick3DMorphTarget::attributes()
{
    return QQmlListProperty<Qt3DRender::QAttribute>(this, 0,
                                       &QQuick3DMorphTarget::appendAttribute,
                                       &QQuick3DMorphTarget::attributeCount,
                                       &QQuick3DMorphTarget::attributeAt,
                                       &QQuick3DMorphTarget::clearAttributes);
}

void QQuick3DMorphTarget::appendAttribute(QQmlListProperty<Qt3DRender::QAttribute> *list, Qt3DRender::QAttribute *bar)
{
    QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
    if (target)
        target->parentMorphTarget()->addAttribute(bar);
}

int QQuick3DMorphTarget::attributeCount(QQmlListProperty<Qt3DRender::QAttribute> *list)
{
    QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
    if (target)
        return target->parentMorphTarget()->attributeList().count();
    return 0;
}

Qt3DRender::QAttribute *QQuick3DMorphTarget::attributeAt(QQmlListProperty<Qt3DRender::QAttribute> *list, int index)
{
    QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
    if (target)
        return qobject_cast<Qt3DRender::QAttribute *>(target->parentMorphTarget()->attributeList().at(index));
    return nullptr;
}

void QQuick3DMorphTarget::clearAttributes(QQmlListProperty<Qt3DRender::QAttribute> *list)
{
    QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
    if (target) {
        QVector<Qt3DRender::QAttribute *> emptyList;
        target->parentMorphTarget()->setAttributes(emptyList);
    }
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
