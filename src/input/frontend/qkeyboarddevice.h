// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_QKEYBOARDDEVICE_H
#define QT3DINPUT_INPUT_QKEYBOARDDEVICE_H

#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QKeyboardDevicePrivate;

class Q_3DINPUTSHARED_EXPORT QKeyboardDevice : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QKeyboardHandler *activeInput READ activeInput NOTIFY activeInputChanged)

public:
    explicit QKeyboardDevice(QNode *parent = nullptr);
    ~QKeyboardDevice();

    QKeyboardHandler *activeInput() const;

    int axisCount() const final;
    int buttonCount() const final;
    QStringList axisNames() const final;
    QStringList buttonNames() const final;
    int axisIdentifier(const QString &name) const final;
    int buttonIdentifier(const QString &name) const final;

protected:
    explicit QKeyboardDevice(QKeyboardDevicePrivate &dd, QNode *parent = nullptr);

Q_SIGNALS:
    void activeInputChanged(QKeyboardHandler *activeInput);

private:
    Q_DECLARE_PRIVATE(QKeyboardDevice)
    void setActiveInput(QKeyboardHandler *activeInput);
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_QKEYBOARDDEVICE_H
