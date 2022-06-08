// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QLEVELOFDETAIL_H
#define QT3DRENDER_QLEVELOFDETAIL_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qlevelofdetailboundingsphere.h>
#include <Qt3DRender/qcamera.h>

#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLevelOfDetailPrivate;

class Q_3DRENDERSHARED_EXPORT QLevelOfDetail : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(ThresholdType thresholdType READ thresholdType WRITE setThresholdType NOTIFY thresholdTypeChanged)
    Q_PROPERTY(QList<qreal> thresholds READ thresholds WRITE setThresholds NOTIFY thresholdsChanged)
    Q_PROPERTY(Qt3DRender::QLevelOfDetailBoundingSphere volumeOverride READ volumeOverride WRITE setVolumeOverride NOTIFY volumeOverrideChanged)

public:
    enum ThresholdType {
        DistanceToCameraThreshold,
        ProjectedScreenPixelSizeThreshold,
    };
    Q_ENUM(ThresholdType) // LCOV_EXCL_LINE

    explicit QLevelOfDetail(Qt3DCore::QNode *parent = nullptr);
    ~QLevelOfDetail();

    QCamera *camera() const;
    int currentIndex() const;
    ThresholdType thresholdType() const;
    QList<qreal> thresholds() const;
    QLevelOfDetailBoundingSphere volumeOverride() const;

    Q_INVOKABLE Qt3DRender::QLevelOfDetailBoundingSphere createBoundingSphere(const QVector3D &center, float radius);

public Q_SLOTS:
    void setCamera(QCamera *camera);
    void setCurrentIndex(int currentIndex);
    void setThresholdType(ThresholdType thresholdType);
    void setThresholds(const QList<qreal> &thresholds);
    void setVolumeOverride(const QLevelOfDetailBoundingSphere &volumeOverride);

Q_SIGNALS:
    void cameraChanged(QCamera *camera);
    void currentIndexChanged(int currentIndex);
    void thresholdTypeChanged(ThresholdType thresholdType);
    void thresholdsChanged(const QList<qreal> &thresholds);
    void volumeOverrideChanged(const QLevelOfDetailBoundingSphere &volumeOverride);

protected:
    explicit QLevelOfDetail(QLevelOfDetailPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QLevelOfDetail)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLEVELOFDETAIL_H
