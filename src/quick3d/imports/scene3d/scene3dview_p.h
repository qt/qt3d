/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SCENE3DVIEW_P_H
#define SCENE3DVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/QQuickItem>
#include <QtCore/QFlags>

QT_BEGIN_NAMESPACE

class QSGTexture;

namespace Qt3DCore {
class QEntity;
class QNode;
}

namespace Qt3DRender {

class QLayer;
class QLayerFilter;
class Scene3DItem;
class QFrameGraphNode;
class QViewport;

class Scene3DView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity* entity READ entity WRITE setEntity NOTIFY entityChanged)
    Q_PROPERTY(Qt3DRender::Scene3DItem *scene3D READ scene3D WRITE setScene3D NOTIFY scene3DChanged)
    Q_CLASSINFO("DefaultProperty", "entity")

public:
    enum DirtyFlag {
        DirtyNode = 1 << 0,
        DirtyTexture = 1 << 1
    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlag)

    explicit Scene3DView(QQuickItem *parent = nullptr);
    ~Scene3DView();

    Qt3DCore::QEntity *entity() const;
    Scene3DItem *scene3D() const;

    Qt3DCore::QEntity *viewSubtree() const;
    Qt3DRender::QFrameGraphNode *viewFrameGraph() const;

    void setTexture(QSGTexture *texture);
    QSGTexture *texture() const;

    void markSGNodeDirty();

public Q_SLOTS:
    void setEntity(Qt3DCore::QEntity *entity);
    void setScene3D(Scene3DItem *scene3D);

Q_SIGNALS:
    void entityChanged();
    void scene3DChanged();

private:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData) override;
    void adoptSubtree(Qt3DCore::QEntity *subtree);
    void abandonSubtree(Qt3DCore::QEntity *subtree);

    Scene3DItem *m_scene3D;
    Qt3DCore::QEntity *m_entity;
    Qt3DCore::QNode *m_previousFGParent;

    Qt3DCore::QEntity *m_holderEntity;
    Qt3DRender::QLayer *m_holderLayer;
    Qt3DRender::QLayerFilter *m_holderLayerFilter;
    Qt3DRender::QViewport *m_holderViewport;

    QMetaObject::Connection m_scene3DDestroyedConnection;

    DirtyFlags m_dirtyFlags;
    QSGTexture *m_texture;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Scene3DView::DirtyFlags)


} // Qt3DRender

QT_END_NAMESPACE

#endif // SCENE3DVIEW_P_H
