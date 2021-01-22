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
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged REVISION 9)

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
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QVIEWPORT_H
