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
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(bool inverseXTranslate READ inverseXTranslate WRITE setInverseXTranslate NOTIFY inverseXTranslateChanged)
    Q_PROPERTY(bool inverseYTranslate READ inverseYTranslate WRITE setInverseYTranslate NOTIFY inverseYTranslateChanged)
    Q_PROPERTY(bool inversePan READ inversePan WRITE setInversePan NOTIFY inversePanChanged)
    Q_PROPERTY(bool inverseTilt READ inverseTilt WRITE setInverseTilt NOTIFY inverseTiltChanged)
    Q_PROPERTY(bool zoomTranslateViewCenter READ zoomTranslateViewCenter WRITE setZoomTranslateViewCenter NOTIFY zoomTranslateViewCenterChanged)

public:
    explicit QOrbitCameraController(Qt3DCore::QNode *parent = nullptr);
    ~QOrbitCameraController();

    float zoomInLimit() const;

    void setZoomInLimit(float zoomInLimit);

    QVector3D upVector() const;

    void setUpVector(const QVector3D& upVector);

    bool inverseXTranslate() const;

    void setInverseXTranslate(bool isInverse);

    bool inverseYTranslate() const;

    void setInverseYTranslate(bool isInverse);

    bool inversePan() const;

    void setInversePan(bool isInverse);

    bool inverseTilt() const;

    void setInverseTilt(bool isInverse);

    bool zoomTranslateViewCenter() const;

    void setZoomTranslateViewCenter(bool isTranslate);

Q_SIGNALS:
    void zoomInLimitChanged();

    void upVectorChanged(const QVector3D &upVector);

    void inverseXTranslateChanged(bool isInverse);

    void inverseYTranslateChanged(bool isInverse);

    void inversePanChanged(bool isInverse);

    void inverseTiltChanged(bool isInverse);

    void zoomTranslateViewCenterChanged(bool isTranslate);

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
