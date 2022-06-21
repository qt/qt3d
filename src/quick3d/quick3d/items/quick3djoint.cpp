/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "quick3djoint_p.h"
#include <Qt3DCore/qjoint.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

Quick3DJoint::Quick3DJoint(QObject *parent)
    : QObject(parent)
{
}

/*!
    \qmlproperty list<Joint> Qt3DCore::Joint::childJoints
    \readonly
*/
QQmlListProperty<QJoint> Quick3DJoint::childJoints()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QJoint;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *joint) {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        jointExtension->parentJoint()->addChildJoint(joint);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        return jointExtension->parentJoint()->childJoints().count();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        return jointExtension->parentJoint()->childJoints().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        const auto joints = jointExtension->parentJoint()->childJoints();
        for (QJoint *joint : joints)
            jointExtension->parentJoint()->removeChildJoint(joint);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3djoint_p.cpp"
