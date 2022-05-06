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

#ifndef QT3DCORE_QSKELETON_H
#define QT3DCORE_QSKELETON_H

#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSkeletonPrivate;

class Q_3DCORESHARED_EXPORT QSkeleton : public QAbstractSkeleton
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QJoint* rootJoint READ rootJoint WRITE setRootJoint NOTIFY rootJointChanged)

public:
    QSkeleton(Qt3DCore::QNode *parent = nullptr);
    ~QSkeleton();

    Qt3DCore::QJoint *rootJoint() const;

public Q_SLOTS:
    void setRootJoint(Qt3DCore::QJoint *rootJoint);

Q_SIGNALS:
    void rootJointChanged(Qt3DCore::QJoint *rootJoint);

private:
    Q_DECLARE_PRIVATE(QSkeleton)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSKELETON_H
