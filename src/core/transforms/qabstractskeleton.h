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
    // TODO Unused remove in Qt6
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QAbstractSkeleton)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTSKELETON_H
