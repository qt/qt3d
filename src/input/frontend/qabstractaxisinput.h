/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_QABSTRACTAXISINPUT_H
#define QT3DINPUT_QABSTRACTAXISINPUT_H

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;
class QAbstractAxisInputPrivate;

class Q_3DINPUTSHARED_EXPORT QAbstractAxisInput : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QAbstractPhysicalDevice *sourceDevice READ sourceDevice WRITE setSourceDevice NOTIFY sourceDeviceChanged)

public:
    ~QAbstractAxisInput();

    QAbstractPhysicalDevice *sourceDevice() const;

public Q_SLOTS:
    void setSourceDevice(QAbstractPhysicalDevice *sourceDevice);

Q_SIGNALS:
    void sourceDeviceChanged(QAbstractPhysicalDevice *sourceDevice);

protected:
    QAbstractAxisInput(QAbstractAxisInputPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractAxisInput)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QQT3DINPUT_ABSTRACTAXISINPUT_H
