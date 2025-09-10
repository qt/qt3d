// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCLIPLOADER_H
#define QT3DANIMATION_QANIMATIONCLIPLOADER_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClipLoaderPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAnimationClipLoader : public QAbstractAnimationClip
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    explicit QAnimationClipLoader(Qt3DCore::QNode *parent = nullptr);
    explicit QAnimationClipLoader(const QUrl &source,
                                  Qt3DCore::QNode *parent = nullptr);
    ~QAnimationClipLoader();

    enum Status {
        NotReady = 0,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    QUrl source() const;
    Status status() const;

public Q_SLOTS:
    void setSource(const QUrl &source);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(Status status);

protected:
    explicit QAnimationClipLoader(QAnimationClipLoaderPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAnimationClipLoader)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCLIPLOADER_H
