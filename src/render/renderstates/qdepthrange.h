// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
