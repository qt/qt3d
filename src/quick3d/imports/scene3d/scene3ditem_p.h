/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#ifndef SCENE3DITEM_H
#define SCENE3DITEM_H

#include <QQuickItem>

QT_BEGIN_NAMESPACE

namespace Qt3D
{
class QAspectEngine;
class QEntity;
class QRenderAspect;

class Scene3DRenderer;
class Scene3DCleaner;

class Scene3DItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QEntity* entity READ entity WRITE setEntity NOTIFY entityChanged)
    Q_PROPERTY(QStringList aspects READ aspects WRITE setAspects NOTIFY aspectsChanged)
    Q_PROPERTY(bool multisample READ multisample WRITE setMultisample NOTIFY multisampleChanged)
    Q_CLASSINFO("DefaultProperty", "entity")
public:
    explicit Scene3DItem(QQuickItem *parent = 0);
    ~Scene3DItem();

    QStringList aspects() const;
    Qt3D::QEntity *entity() const;

    bool multisample() const;
    void setMultisample(bool enable);

public Q_SLOTS:
    void setAspects(const QStringList &aspects);
    void setEntity(Qt3D::QEntity *entity);

Q_SIGNALS:
    void aspectsChanged();
    void entityChanged();
    void multisampleChanged();

private Q_SLOTS:
    void applyRootEntityChange();

private:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData) Q_DECL_OVERRIDE;

    QStringList m_aspects;
    Qt3D::QEntity *m_entity;

    Qt3D::QAspectEngine *m_aspectEngine;
    Qt3D::QRenderAspect *m_renderAspect;
    Scene3DRenderer *m_renderer;
    Scene3DCleaner *m_rendererCleaner;

    bool m_multisample;
};

} // Qt3D

QT_END_NAMESPACE

#endif
