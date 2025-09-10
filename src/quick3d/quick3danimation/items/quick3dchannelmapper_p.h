// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_QUICK_QUICK3DBASICANIMATION_H
#define QT3DANIMATION_ANIMATION_QUICK_QUICK3DBASICANIMATION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DQuickAnimation/private/qt3dquickanimation_global_p.h>
#include <Qt3DAnimation/qabstractchannelmapping.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <QQmlListProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {
namespace Quick {

class Q_3DQUICKANIMATIONSHARED_PRIVATE_EXPORT Quick3DChannelMapper  : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DAnimation::QAbstractChannelMapping> mappings READ qmlMappings CONSTANT)
    Q_CLASSINFO("DefaultProperty", "mappings")

public:
    explicit Quick3DChannelMapper(QObject *parent = nullptr);

    inline QChannelMapper *parentMapper() const { return qobject_cast<QChannelMapper *>(parent()); }
    QQmlListProperty<QAbstractChannelMapping> qmlMappings();
};

} // namespace Quick
} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_QUICK_QUICK3DBASICANIMATION_H
