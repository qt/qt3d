/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#include "quick3dmorphtarget_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DMorphTarget::QQuick3DMorphTarget(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DCore::QAttribute> QQuick3DMorphTarget::attributes()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DCore::QAttribute;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target)
            target->parentMorphTarget()->addAttribute(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target)
            return target->parentMorphTarget()->attributeList().count();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target)
            return qobject_cast<Qt3DCore::QAttribute *>(target->parentMorphTarget()->attributeList().at(index));
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target) {
            QList<Qt3DCore::QAttribute *> emptyList;
            target->parentMorphTarget()->setAttributes(emptyList);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3dmorphtarget_p.cpp"
