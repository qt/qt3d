// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H
#define QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H

#include <Qt3DQuickScene2D/qt3dquickscene2d_global.h>
#include <Qt3DRender/qrendertargetoutput.h>

#include <QtCore/QUrl>
#include <QtCore/QEvent>

#include <Qt3DCore/qentity.h>

#include <QtQuick/qquickitem.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Quick {

class QScene2DPrivate;

class Q_3DQUICKSCENE2DSHARED_EXPORT QScene2D : public Qt3DCore::QNode
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QRenderTargetOutput *output READ output WRITE setOutput NOTIFY outputChanged)
    Q_PROPERTY(RenderPolicy renderPolicy READ renderPolicy WRITE setRenderPolicy NOTIFY renderPolicyChanged)
    Q_PROPERTY(QQuickItem *item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(bool mouseEnabled READ isMouseEnabled WRITE setMouseEnabled NOTIFY mouseEnabledChanged)

    Q_CLASSINFO("DefaultProperty", "item")

public:

    enum RenderPolicy {
        Continuous,
        SingleShot
    };
    Q_ENUM(RenderPolicy)

    explicit QScene2D(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QRenderTargetOutput *output() const;
    QScene2D::RenderPolicy renderPolicy() const;
    QQuickItem *item() const;
    bool isMouseEnabled() const;

    QList<Qt3DCore::QEntity *> entities() const;
    void addEntity(Qt3DCore::QEntity *entity);
    void removeEntity(Qt3DCore::QEntity *entity);

public Q_SLOTS:
    void setOutput(Qt3DRender::QRenderTargetOutput *output);
    void setRenderPolicy(QScene2D::RenderPolicy policy);
    void setItem(QQuickItem *item);
    void setMouseEnabled(bool enabled);

Q_SIGNALS:
    void outputChanged(Qt3DRender::QRenderTargetOutput *output);
    void renderPolicyChanged(QScene2D::RenderPolicy policy);
    void itemChanged(QQuickItem *item);
    void mouseEnabledChanged(bool enabled);

protected:
    Q_DECLARE_PRIVATE(QScene2D)
};

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H
