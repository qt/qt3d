// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERPASSFILTER_H
#define QT3DRENDER_QRENDERPASSFILTER_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qframegraphnode.h>
#include <QtCore/QString>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderPassFilterPrivate;
class QFilterKey;
class QParameter;

class Q_3DRENDERSHARED_EXPORT QRenderPassFilter : public QFrameGraphNode
{
    Q_OBJECT

public:
    explicit QRenderPassFilter(Qt3DCore::QNode *parent = nullptr);
    ~QRenderPassFilter();

    QList<QFilterKey *> matchAny() const;
    void addMatch(QFilterKey *filterKey);
    void removeMatch(QFilterKey *filterKey);

    void addParameter(QParameter *parameter);
    void removeParameter(QParameter *parameter);
    QList<QParameter *> parameters() const;

protected:
    explicit QRenderPassFilter(QRenderPassFilterPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderPassFilter)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERPASSFILTER_H
