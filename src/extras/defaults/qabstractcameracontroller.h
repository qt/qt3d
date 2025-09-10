// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_H
#define QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_H

#include <Qt3DCore/QEntity>
#include <Qt3DExtras/qt3dextras_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
class QKeyboardDevice;
class QMouseDevice;
}

namespace Qt3DRender {
class QCamera;
}

namespace Qt3DExtras {

class QAbstractCameraControllerPrivate;

class Q_3DEXTRASSHARED_EXPORT QAbstractCameraController : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged)
    Q_PROPERTY(float lookSpeed READ lookSpeed WRITE setLookSpeed NOTIFY lookSpeedChanged)
    Q_PROPERTY(float acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(float deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)

public:
    ~QAbstractCameraController();

    Qt3DRender::QCamera *camera() const;
    float linearSpeed() const;
    float lookSpeed() const;

    float acceleration() const;
    float deceleration() const;

    void setCamera(Qt3DRender::QCamera *camera);
    void setLinearSpeed(float linearSpeed);
    void setLookSpeed(float lookSpeed);

    void setAcceleration(float acceleration);
    void setDeceleration(float deceleration);

Q_SIGNALS:
    void cameraChanged();
    void linearSpeedChanged();
    void lookSpeedChanged();

    void accelerationChanged(float acceleration);
    void decelerationChanged(float deceleration);

protected:
    explicit QAbstractCameraController(Qt3DCore::QNode *parent = nullptr);
    QAbstractCameraController(QAbstractCameraControllerPrivate &dd, Qt3DCore::QNode *parent = nullptr);

    Qt3DInput::QKeyboardDevice *keyboardDevice() const;
    Qt3DInput::QMouseDevice *mouseDevice() const;

public:
    struct InputState
    {
        float rxAxisValue;
        float ryAxisValue;
        float txAxisValue;
        float tyAxisValue;
        float tzAxisValue;

        bool leftMouseButtonActive;
        bool middleMouseButtonActive;
        bool rightMouseButtonActive;

        bool altKeyActive;
        bool shiftKeyActive;
    };

private:
    virtual void moveCamera(const InputState &state, float dt) = 0;

private:
    Q_DECLARE_PRIVATE(QAbstractCameraController)
};

}   // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_H
