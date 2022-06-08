// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTSKELETON_H
#define QT3DCORE_QABSTRACTSKELETON_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractSkeletonPrivate;

class Q_3DCORESHARED_EXPORT QAbstractSkeleton : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(int jointCount READ jointCount NOTIFY jointCountChanged)

public:
    ~QAbstractSkeleton();

    int jointCount() const;

Q_SIGNALS:
    void jointCountChanged(int jointCount);

protected:
    QAbstractSkeleton(QAbstractSkeletonPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractSkeleton)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTSKELETON_H
