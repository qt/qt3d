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
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged REVISION(6, 7))
    Q_PROPERTY(bool inverseXTranslate READ inverseXTranslate WRITE setInverseXTranslate NOTIFY inverseXTranslateChanged REVISION(6, 7))
    Q_PROPERTY(bool inverseYTranslate READ inverseYTranslate WRITE setInverseYTranslate NOTIFY inverseYTranslateChanged REVISION(6, 7))
    Q_PROPERTY(bool inversePan READ inversePan WRITE setInversePan NOTIFY inversePanChanged REVISION(6, 7))
    Q_PROPERTY(bool inverseTilt READ inverseTilt WRITE setInverseTilt NOTIFY inverseTiltChanged REVISION(6, 7))
    Q_PROPERTY(bool zoomTranslateViewCenter READ zoomTranslateViewCenter WRITE setZoomTranslateViewCenter NOTIFY zoomTranslateViewCenterChanged REVISION(6, 7))

public:
    explicit QOrbitCameraController(Qt3DCore::QNode *parent = nullptr);
    ~QOrbitCameraController();

    float zoomInLimit() const;
    QVector3D upVector() const;
    bool inverseXTranslate() const;
    bool inverseYTranslate() const;
    bool inversePan() const;
    bool inverseTilt() const;
    bool zoomTranslateViewCenter() const;

public Q_SLOTS:
    void setZoomInLimit(float zoomInLimit);
    void setUpVector(const QVector3D& upVector);
    void setInverseXTranslate(bool isInverse);
    void setInverseYTranslate(bool isInverse);
    void setInversePan(bool isInverse);
    void setInverseTilt(bool isInverse);
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

