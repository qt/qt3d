/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

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
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPOLYGONOFFSET_H
