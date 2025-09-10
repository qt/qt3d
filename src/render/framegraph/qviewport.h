// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QVIEWPORT_H
#define QT3DRENDER_QVIEWPORT_H

#include <Qt3DRender/qframegraphnode.h>
#include <QtCore/QRectF>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QViewportPrivate;

class Q_3DRENDERSHARED_EXPORT QViewport : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QRectF normalizedRect READ normalizedRect WRITE setNormalizedRect NOTIFY normalizedRectChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged REVISION(2, 9))

public:
    explicit QViewport(Qt3DCore::QNode *parent = nullptr);
    ~QViewport();

    QRectF normalizedRect() const;
    float gamma() const;

public Q_SLOTS:
    void setNormalizedRect(const QRectF& normalizedRect);
    void setGamma(float gamma);

Q_SIGNALS:
    void normalizedRectChanged(const QRectF& normalizedRect);
    void gammaChanged(float gamma);

protected:
    explicit QViewport(QViewportPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QViewport)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QVIEWPORT_H
