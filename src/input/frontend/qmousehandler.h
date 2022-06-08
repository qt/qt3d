// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QMOUSEHANDLER_H
#define QT3DINPUT_QMOUSEHANDLER_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qmouseevent.h>
#include <Qt3DInput/qmousedevice.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QMouseHandlerPrivate;

class Q_3DINPUTSHARED_EXPORT QMouseHandler : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QMouseDevice *sourceDevice READ sourceDevice WRITE setSourceDevice NOTIFY sourceDeviceChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

public:
    explicit QMouseHandler(QNode *parent = nullptr);
    ~QMouseHandler();

    QMouseDevice *sourceDevice() const;
    bool containsMouse() const;

public Q_SLOTS:
    void setSourceDevice(QMouseDevice *mouseDevice);

Q_SIGNALS:
    void sourceDeviceChanged(QMouseDevice *mouseDevice);
    void containsMouseChanged(bool containsMouse);

    void clicked(Qt3DInput::QMouseEvent *mouse);
    void doubleClicked(Qt3DInput::QMouseEvent *mouse);

    void entered();
    void exited();

    void pressed(Qt3DInput::QMouseEvent *mouse);
    void released(Qt3DInput::QMouseEvent *mouse);
    void pressAndHold(Qt3DInput::QMouseEvent *mouse);
    void positionChanged(Qt3DInput::QMouseEvent *mouse);

#if QT_CONFIG(wheelevent)
    void wheel(Qt3DInput::QWheelEvent *wheel);
#endif

protected:
    void setContainsMouse(bool contains);

private:
    Q_DECLARE_PRIVATE(QMouseHandler)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QMOUSEHANDLER_H
