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
****************************************************************************/

#ifndef QT3DCORE_QARMATURE_H
#define QT3DCORE_QARMATURE_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QArmaturePrivate;
class QAbstractSkeleton;

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
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QARMATURE_H
