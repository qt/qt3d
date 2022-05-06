/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qt3dquick_global_p.h"

#include <Qt3DQuick/private/qt3dquicknodefactory_p.h>
#include <Qt3DQuick/private/qt3dquickvaluetypes_p.h>
#include <QtQml/private/qqmlglobal_p.h>
#include <QtQml/private/qv4engine_p.h>
#include <QtQml/private/qv4object_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

static QQmlPrivate::AutoParentResult qquick3ditem_autoParent(QObject *obj, QObject *parent)
{
    // When setting a parent (especially during dynamic object creation) in QML,
    // also try to set up the analogous item/window relationship.
    auto parentNode = qmlobject_cast<Qt3DCore::QNode *>(parent);
    if (parentNode) {
        auto node = qmlobject_cast<Qt3DCore::QNode *>(obj);
        if (node) {
            // A QNode has another QNode child
            node->setParent(parentNode);
            return QQmlPrivate::Parented;
        }
    } else {
        return QQmlPrivate::IncompatibleParent;
    }
    return QQmlPrivate::IncompatibleObject;
}

void Quick3D_initialize()
{
    QAbstractNodeFactory::registerNodeFactory(QuickNodeFactory::instance());

    // Register a hook called when we do component.create() that sets the
    // parent. We need this as QObject::setParent() is insufficient to propagate
    // the arbiter and scene to the children (see QNode::setParent(QNode *).
    // TODO: Replace this with virtual void QObjectPrivate::setParent(QObject *)
    // that can be called from QObject ctor and QObject::setParent(). That would
    // allow removal of this hook here and in QtQuick.
    QQmlPrivate::RegisterAutoParent autoparent = { 0, &qquick3ditem_autoParent };
    QQmlPrivate::qmlregister(QQmlPrivate::AutoParentRegistration, &autoparent);
}

void Quick3D_uninitialize()
{
}

void Quick3D_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickNodeFactory::instance()->registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE
