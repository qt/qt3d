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

#ifndef QT3DINPUT_QACTIONINPUT_H
#define QT3DINPUT_QACTIONINPUT_H

#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;
class QActionInputPrivate;

class Q_3DINPUTSHARED_EXPORT QActionInput : public Qt3DInput::QAbstractActionInput
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QAbstractPhysicalDevice *sourceDevice READ sourceDevice WRITE setSourceDevice NOTIFY sourceDeviceChanged)
    Q_PROPERTY(QVector<int> buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)

public:
    explicit QActionInput(Qt3DCore::QNode *parent = nullptr);
    ~QActionInput();

    QAbstractPhysicalDevice *sourceDevice() const;
    QVector<int> buttons() const;

public Q_SLOTS:
    void setSourceDevice(QAbstractPhysicalDevice *sourceDevice);
    void setButtons(const QVector<int> &buttons);

Q_SIGNALS:
    void sourceDeviceChanged(QAbstractPhysicalDevice *sourceDevice);
    void buttonsChanged(const QVector<int> &buttons);

private:
    Q_DECLARE_PRIVATE(QActionInput)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTIONINPUT_H
