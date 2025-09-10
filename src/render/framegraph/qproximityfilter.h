// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPROXIMITYFILTER_H
#define QT3DRENDER_QPROXIMITYFILTER_H

#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QProximityFilterPrivate;

class Q_3DRENDERSHARED_EXPORT QProximityFilter : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity WRITE setEntity NOTIFY entityChanged)
    Q_PROPERTY(float distanceThreshold READ distanceThreshold WRITE setDistanceThreshold NOTIFY distanceThresholdChanged)

public:
    explicit QProximityFilter(Qt3DCore::QNode *parent = nullptr);
    ~QProximityFilter();

    Qt3DCore::QEntity *entity() const;
    float distanceThreshold() const;

public Q_SLOTS:
    void setEntity(Qt3DCore::QEntity *entity);
    void setDistanceThreshold(float distanceThreshold);

Q_SIGNALS:
    void entityChanged(Qt3DCore::QEntity *entity);
    void distanceThresholdChanged(float distanceThreshold);

protected:
    explicit QProximityFilter(QProximityFilterPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QProximityFilter)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPROXIMITYFILTER_H
