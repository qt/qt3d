// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QSKELETONLOADER_H
#define QT3DCORE_QSKELETONLOADER_H

#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSkeletonLoaderPrivate;

class Q_3DCORESHARED_EXPORT QSkeletonLoader : public QAbstractSkeleton
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool createJointsEnabled READ isCreateJointsEnabled WRITE setCreateJointsEnabled NOTIFY createJointsEnabledChanged)
    Q_PROPERTY(Qt3DCore::QJoint* rootJoint READ rootJoint NOTIFY rootJointChanged)

public:
    explicit QSkeletonLoader(Qt3DCore::QNode *parent = nullptr);
    explicit QSkeletonLoader(const QUrl &source,
                             Qt3DCore::QNode *parent = nullptr);
    ~QSkeletonLoader();

    enum Status {
        NotReady = 0,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    QUrl source() const;
    Status status() const;
    bool isCreateJointsEnabled() const;
    Qt3DCore::QJoint* rootJoint() const;

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setCreateJointsEnabled(bool enabled);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(Status status);
    void createJointsEnabledChanged(bool createJointsEnabled);
    void rootJointChanged(Qt3DCore::QJoint* rootJoint);

protected:
    explicit QSkeletonLoader(QSkeletonLoaderPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSkeletonLoader)
    void setRootJoint(QJoint *rootJoint); // Needed for lifetime management of created joints
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSKELETONLOADER_H
