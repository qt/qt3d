// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QORBITCAMERACONTROLLER_H
#define QT3DEXTRAS_QORBITCAMERACONTROLLER_H

#include <Qt3DExtras/qabstractcameracontroller.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QOrbitCameraControllerPrivate;

class Q_3DEXTRASSHARED_EXPORT QOrbitCameraController : public QAbstractCameraController
{
    Q_OBJECT
    Q_PROPERTY(float zoomInLimit READ zoomInLimit WRITE setZoomInLimit NOTIFY zoomInLimitChanged)

public:
    explicit QOrbitCameraController(Qt3DCore::QNode *parent = nullptr);
    ~QOrbitCameraController();

    float zoomInLimit() const;

    void setZoomInLimit(float zoomInLimit);

Q_SIGNALS:
    void zoomInLimitChanged();

protected:
    QOrbitCameraController(QOrbitCameraControllerPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;

private:
    Q_DECLARE_PRIVATE(QOrbitCameraController)
};

}   // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QORBITCAMERACONTROLLER_H
