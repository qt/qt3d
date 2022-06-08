// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QDISPATCHCOMPUTE_H
#define QT3DRENDER_QDISPATCHCOMPUTE_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDispatchComputePrivate;

class Q_3DRENDERSHARED_EXPORT QDispatchCompute : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(int workGroupX READ workGroupX WRITE setWorkGroupX NOTIFY workGroupXChanged)
    Q_PROPERTY(int workGroupY READ workGroupY WRITE setWorkGroupY NOTIFY workGroupYChanged)
    Q_PROPERTY(int workGroupZ READ workGroupZ WRITE setWorkGroupZ NOTIFY workGroupZChanged)
public:
    explicit QDispatchCompute(Qt3DCore::QNode *parent = nullptr);
    ~QDispatchCompute();

    int workGroupX() const;
    int workGroupY() const;
    int workGroupZ() const;

public Q_SLOTS:
    void setWorkGroupX(int workGroupX);
    void setWorkGroupY(int workGroupY);
    void setWorkGroupZ(int workGroupZ);

Q_SIGNALS:
    void workGroupXChanged();
    void workGroupYChanged();
    void workGroupZChanged();

private:
    Q_DECLARE_PRIVATE(QDispatchCompute)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QDISPATCHCOMPUTE_H
