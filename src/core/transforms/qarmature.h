// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QARMATURE_H
#define QT3DCORE_QARMATURE_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QArmaturePrivate;

class Q_3DCORESHARED_EXPORT QArmature : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QAbstractSkeleton* skeleton READ skeleton WRITE setSkeleton NOTIFY skeletonChanged)

public:
    explicit QArmature(Qt3DCore::QNode *parent = nullptr);
    ~QArmature();

    QAbstractSkeleton* skeleton() const;

public Q_SLOTS:
    void setSkeleton(Qt3DCore::QAbstractSkeleton* skeleton);

Q_SIGNALS:
    void skeletonChanged(Qt3DCore::QAbstractSkeleton* skeleton);

protected:
    QArmature(QArmaturePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QArmature)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QARMATURE_H
