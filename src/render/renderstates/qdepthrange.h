/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QDEPTHRANGE_H
#define QT3DRENDER_QDEPTHRANGE_H

#include <Qt3DRender/qrenderstate.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDepthRangePrivate;

class Q_3DRENDERSHARED_EXPORT QDepthRange : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(double nearValue READ nearValue WRITE setNearValue NOTIFY nearValueChanged)
    Q_PROPERTY(double farValue READ farValue WRITE setFarValue NOTIFY farValueChanged)
public:
    explicit QDepthRange(Qt3DCore::QNode *parent = nullptr);
    ~QDepthRange();

    double nearValue() const;
    double farValue() const;

public Q_SLOTS:
    void setNearValue(double value);
    void setFarValue(double value);

Q_SIGNALS:
    void nearValueChanged(double nearValue);
    void farValueChanged(double farValue);

private:
    Q_DECLARE_PRIVATE(QDepthRange)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QDEPTHRANGE_H
