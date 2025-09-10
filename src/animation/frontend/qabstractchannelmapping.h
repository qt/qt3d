// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTCHANNELMAPPING_H
#define QT3DANIMATION_QABSTRACTCHANNELMAPPING_H

#include <Qt3DCore/qnode.h>
#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractChannelMappingPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAbstractChannelMapping : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    ~QAbstractChannelMapping();

protected:
    QAbstractChannelMapping(QAbstractChannelMappingPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractChannelMapping)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTCHANNELMAPPING_H
