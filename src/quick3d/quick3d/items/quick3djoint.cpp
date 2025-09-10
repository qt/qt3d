// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    \qmlproperty list<Joint> Qt3D.Core::Joint::childJoints
    \readonly
*/
QQmlListProperty<QJoint> Quick3DJoint::childJoints()
{
    using qt_size_type = qsizetype;
    using ListContentType = QJoint;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *joint) {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        jointExtension->parentJoint()->addChildJoint(joint);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DJoint *jointExtension = qobject_cast<Quick3DJoint *>(list->object);
        return jointExtension->parentJoint()->childJoints().size();
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
