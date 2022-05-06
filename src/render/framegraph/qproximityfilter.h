/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

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
