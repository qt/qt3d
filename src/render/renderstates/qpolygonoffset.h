// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPOLYGONOFFSET_H
#define QT3DRENDER_QPOLYGONOFFSET_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPolygonOffsetPrivate;

class Q_3DRENDERSHARED_EXPORT QPolygonOffset : public QRenderState
{
    Q_OBJECT

    Q_PROPERTY(float scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(float depthSteps READ depthSteps WRITE setDepthSteps NOTIFY depthStepsChanged)
public:
    explicit QPolygonOffset(Qt3DCore::QNode *parent = nullptr);
    ~QPolygonOffset();

    float scaleFactor() const;
    float depthSteps() const;

public Q_SLOTS:
    void setScaleFactor(float scaleFactor);
    void setDepthSteps(float depthSteps);

Q_SIGNALS:
    void scaleFactorChanged(float scaleFactor);
    void depthStepsChanged(float depthSteps);

private:
    Q_DECLARE_PRIVATE(QPolygonOffset)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPOLYGONOFFSET_H
