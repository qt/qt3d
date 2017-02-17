/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H
#define QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H

#include <Qt3DQuickScene2D/qt3dquickscene2d_global.h>
#include <Qt3DRender/qrendertargetoutput.h>

#include <QtCore/QUrl>
#include <QtCore/QEvent>

#include <Qt3DCore/qnode.h>

#include <QtQuick/qquickitem.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Quick {

class QScene2DPrivate;

class QT3DQUICKSCENE2DSHARED_EXPORT QScene2D : public Qt3DCore::QNode
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QRenderTargetOutput *output READ output WRITE setOutput NOTIFY outputChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QScene2D::RenderPolicy renderPolicy READ renderPolicy WRITE setRenderPolicy NOTIFY renderPolicyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(QQuickItem *item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(bool grabMouse READ isGrabMouseEnabled WRITE setGrabMouseEnabled NOTIFY grabMouseChanged)

    Q_CLASSINFO("DefaultProperty", "item")

public:

    enum RenderPolicy {
        Continuous,
        SingleShot
    };
    Q_ENUM(RenderPolicy)

    explicit QScene2D(Qt3DCore::QNode *parent = nullptr);
    QScene2D(QQmlEngine *engine, Qt3DCore::QNode *parent = nullptr);
    ~QScene2D();

    Qt3DRender::QRenderTargetOutput *output() const;
    QUrl source() const;
    bool loaded() const;
    QScene2D::RenderPolicy renderPolicy() const;
    QQuickItem *item() const;
    QQmlEngine *engine() const;
    bool isGrabMouseEnabled() const;

    bool event(QEvent *event) Q_DECL_OVERRIDE;

    QVector<Qt3DCore::QEntity *> entities();
    void addEntity(Qt3DCore::QEntity *entity);
    void removeEntity(Qt3DCore::QEntity *entity);

public Q_SLOTS:
    void setOutput(Qt3DRender::QRenderTargetOutput *output);
    void setSource(const QUrl &url);
    void setRenderPolicy(QScene2D::RenderPolicy policy);
    void setItem(QQuickItem *item);
    void setGrabMouseEnabled(bool grab);

Q_SIGNALS:
    void outputChanged(Qt3DRender::QRenderTargetOutput *output);
    void sourceChanged(const QUrl &url);
    void loadedChanged(bool loaded);
    void renderPolicyChanged(QScene2D::RenderPolicy policy);
    void itemChanged(QQuickItem *item);
    void grabMouseChanged(bool grab);

protected:
    Q_DECLARE_PRIVATE(QScene2D)

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const Q_DECL_OVERRIDE;

    void sourceLoaded();
};

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_H
