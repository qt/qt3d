// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DCore::QAttribute;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target)
            target->parentMorphTarget()->addAttribute(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DMorphTarget *target = qobject_cast<QQuick3DMorphTarget *>(list->object);
        if (target)
            return target->parentMorphTarget()->attributeList().size();
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
