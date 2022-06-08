// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTRAYCASTER_H
#define QT3DRENDER_QABSTRACTRAYCASTER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qraycasterhit.h>
#include <Qt3DRender/qt3drender_global.h>

#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractRayCasterPrivate;
class QLayer;

class Q_3DRENDERSHARED_EXPORT QAbstractRayCaster : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(RunMode runMode READ runMode WRITE setRunMode NOTIFY runModeChanged)
    Q_PROPERTY(FilterMode filterMode READ filterMode WRITE setFilterMode NOTIFY filterModeChanged)
    Q_PROPERTY(Hits hits READ hits NOTIFY hitsChanged)
public:
    enum RunMode {
        Continuous,
        SingleShot
    };
    Q_ENUM(RunMode)

    enum FilterMode {
        AcceptAnyMatchingLayers = 0,
        AcceptAllMatchingLayers,
        DiscardAnyMatchingLayers,
        DiscardAllMatchingLayers,
    };
    Q_ENUM(FilterMode) // LOVC_EXLC_LINE

    using Hits = QList<QRayCasterHit>;

    explicit QAbstractRayCaster(QNode *parent = nullptr);
    ~QAbstractRayCaster();

    RunMode runMode() const;
    FilterMode filterMode() const;
    Hits hits() const;

    void addLayer(QLayer *layer);
    void removeLayer(QLayer *layer);
    QList<QLayer *> layers() const;

public Q_SLOTS:
    void setRunMode(RunMode runMode);
    void setFilterMode(FilterMode filterMode);

Q_SIGNALS:
    void runModeChanged(Qt3DRender::QAbstractRayCaster::RunMode runMode);
    void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits &hits);
    void filterModeChanged(Qt3DRender::QAbstractRayCaster::FilterMode filterMode);

protected:
    explicit QAbstractRayCaster(QAbstractRayCasterPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractRayCaster)
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QAbstractRayCaster::Hits) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QABSTRACTRAYCASTER_H
