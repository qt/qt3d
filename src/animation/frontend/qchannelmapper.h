// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCHANNELMAPPER_H
#define QT3DANIMATION_QCHANNELMAPPER_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelMapperPrivate;
class QAbstractChannelMapping;

class Q_3DANIMATIONSHARED_EXPORT QChannelMapper : public Qt3DCore::QNode
{
    Q_OBJECT

public:
    explicit QChannelMapper(Qt3DCore::QNode *parent = nullptr);
    ~QChannelMapper();

    void addMapping(QAbstractChannelMapping *mapping);
    void removeMapping(QAbstractChannelMapping *mapping);
    QList<QAbstractChannelMapping *> mappings() const;

protected:
    explicit QChannelMapper(QChannelMapperPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QChannelMapper)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCHANNELMAPPER_H
