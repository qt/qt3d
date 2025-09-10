// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QLAYERFILTER_H
#define QT3DRENDER_QLAYERFILTER_H

#include <Qt3DRender/qframegraphnode.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLayer;
class QLayerFilterPrivate;

class Q_3DRENDERSHARED_EXPORT QLayerFilter : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(FilterMode filterMode READ filterMode WRITE setFilterMode NOTIFY filterModeChanged)
public:
    enum FilterMode
    {
        AcceptAnyMatchingLayers = 0,
        AcceptAllMatchingLayers,
        DiscardAnyMatchingLayers,
        DiscardAllMatchingLayers,
    };
    Q_ENUM(FilterMode) // LOVC_EXLC_LINE

    explicit QLayerFilter(Qt3DCore::QNode *parent = nullptr);
    ~QLayerFilter();

    void addLayer(QLayer *layer);
    void removeLayer(QLayer *layer);
    QList<QLayer *> layers() const;

    FilterMode filterMode() const;
    void setFilterMode(FilterMode filterMode);

Q_SIGNALS:
    void filterModeChanged(FilterMode filterMode);


protected:
    explicit QLayerFilter(QLayerFilterPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QLayerFilter)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLAYERFILTER_H
