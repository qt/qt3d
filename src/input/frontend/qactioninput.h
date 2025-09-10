// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QACTIONINPUT_H
#define QT3DINPUT_QACTIONINPUT_H

#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QActionInputPrivate;

class Q_3DINPUTSHARED_EXPORT QActionInput : public Qt3DInput::QAbstractActionInput
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QAbstractPhysicalDevice *sourceDevice READ sourceDevice WRITE setSourceDevice NOTIFY sourceDeviceChanged)
    Q_PROPERTY(QList<int> buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)

public:
    explicit QActionInput(Qt3DCore::QNode *parent = nullptr);
    ~QActionInput();

    QAbstractPhysicalDevice *sourceDevice() const;
    QList<int> buttons() const;

public Q_SLOTS:
    void setSourceDevice(QAbstractPhysicalDevice *sourceDevice);
    void setButtons(const QList<int> &buttons);

Q_SIGNALS:
    void sourceDeviceChanged(QAbstractPhysicalDevice *sourceDevice);
    void buttonsChanged(const QList<int> &buttons);

private:
    Q_DECLARE_PRIVATE(QActionInput)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTIONINPUT_H
