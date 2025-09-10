// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCALLBACKMAPPING_H
#define QT3DANIMATION_QCALLBACKMAPPING_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qanimationcallback.h>
#include <Qt3DAnimation/qabstractchannelmapping.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QCallbackMappingPrivate;

class Q_3DANIMATIONSHARED_EXPORT QCallbackMapping : public QAbstractChannelMapping
{
    Q_OBJECT
    Q_PROPERTY(QString channelName READ channelName WRITE setChannelName NOTIFY channelNameChanged)

public:
    explicit QCallbackMapping(Qt3DCore::QNode *parent = nullptr);
    ~QCallbackMapping();

    QString channelName() const;
    QAnimationCallback *callback() const;

    void setCallback(int type, QAnimationCallback *callback, QAnimationCallback::Flags flags = QAnimationCallback::OnOwningThread);

public Q_SLOTS:
    void setChannelName(const QString &channelName);

Q_SIGNALS:
    void channelNameChanged(QString channelName);

protected:
    explicit QCallbackMapping(QCallbackMappingPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QCallbackMapping)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCALLBACKMAPPING_H
