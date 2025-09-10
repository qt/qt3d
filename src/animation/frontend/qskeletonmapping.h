// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QSKELETONMAPPING_H
#define QT3DANIMATION_QSKELETONMAPPING_H

#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DAnimation/qabstractchannelmapping.h>
#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QSkeletonMappingPrivate;

class Q_3DANIMATIONSHARED_EXPORT QSkeletonMapping : public QAbstractChannelMapping
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QAbstractSkeleton* skeleton READ skeleton WRITE setSkeleton NOTIFY skeletonChanged)

public:
    explicit QSkeletonMapping(Qt3DCore::QNode *parent = nullptr);
    ~QSkeletonMapping();

    Qt3DCore::QAbstractSkeleton *skeleton() const;

public Q_SLOTS:
    void setSkeleton(Qt3DCore::QAbstractSkeleton *skeleton);

Q_SIGNALS:
    void skeletonChanged(Qt3DCore::QAbstractSkeleton *skeleton);

protected:
    QSkeletonMapping(QSkeletonMappingPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSkeletonMapping)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QSKELETONMAPPING_H
