/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#ifndef QT3DANIMATION_QCHANNELMAPPING_H
#define QT3DANIMATION_QCHANNELMAPPING_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qanimationcallback.h>
#include <Qt3DAnimation/qabstractchannelmapping.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelMappingPrivate;

class Q_3DANIMATIONSHARED_EXPORT QChannelMapping : public QAbstractChannelMapping
{
    Q_OBJECT
    Q_PROPERTY(QString channelName READ channelName WRITE setChannelName NOTIFY channelNameChanged)
    Q_PROPERTY(Qt3DCore::QNode *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)

public:
    explicit QChannelMapping(Qt3DCore::QNode *parent = nullptr);
    ~QChannelMapping();

    QString channelName() const;
    Qt3DCore::QNode *target() const;
    QString property() const;

public Q_SLOTS:
    void setChannelName(const QString &channelName);
    void setTarget(Qt3DCore::QNode *target);
    void setProperty(const QString &property);

Q_SIGNALS:
    void channelNameChanged(QString channelName);
    void targetChanged(Qt3DCore::QNode *target);
    void propertyChanged(QString property);

protected:
    explicit QChannelMapping(QChannelMappingPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QChannelMapping)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCHANNELMAPPING_H
