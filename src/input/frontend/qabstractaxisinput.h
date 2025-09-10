// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
