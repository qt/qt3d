/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#if 0

#include "qboundingvolumedebug_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/qspheremesh.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qphongalphamaterial.h>
#include <Qt3DRender/qlayer.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBoundingVolumeDebugPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QBoundingVolumeDebugPrivate()
        : QComponentPrivate()
        , m_recursive(false)
        , m_debugSubtree(nullptr)
        , m_sphereMesh(nullptr)
        , m_transform(nullptr)
        , m_material(nullptr)
        , m_layer(nullptr)
        , m_bvRadius(0.0f)
    {
        m_shareable = false;
    }

    Q_DECLARE_PUBLIC(QBoundingVolumeDebug)

    bool m_recursive;
    Qt3DCore::QEntity *m_debugSubtree;
    Qt3DRender::QSphereMesh *m_sphereMesh;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QMaterial *m_material;
    Qt3DRender::QLayer *m_layer;

    float m_bvRadius;
    QVector3D m_bvCenter;

    Qt3DCore::QEntity *findRootEntity(Qt3DCore::QEntity *e);
    void updateSubtree();
};

QBoundingVolumeDebug::QBoundingVolumeDebug(Qt3DCore::QNode *parent)
    : QComponent(*new QBoundingVolumeDebugPrivate(), parent)
{
}

QBoundingVolumeDebug::~QBoundingVolumeDebug()
{
    QComponent::cleanup();
}

void QBoundingVolumeDebug::setRecursive(bool recursive)
{
    Q_D(QBoundingVolumeDebug);
    if (d->m_recursive != recursive) {
        d->m_recursive = recursive;
        emit recursiveChanged(recursive);
    }
}

bool QBoundingVolumeDebug::recursive() const
{
    Q_D(const QBoundingVolumeDebug);
    return d->m_recursive;
}

void QBoundingVolumeDebug::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QBoundingVolumeDebug);
    Qt3DCore::QPropertyUpdatedChangePtr e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
    if (e->type() == Qt3DCore::PropertyUpdated) {
        if (e->propertyName() == QByteArrayLiteral("center")) {
            d->m_bvCenter = e->value().value<QVector3D>();
            d->updateSubtree();
        } else if (e->propertyName() == QByteArrayLiteral("radius")) {
            d->m_bvRadius = e->value().toFloat();
            d->updateSubtree();
        }
    }
}

// Executed in the frontend thread
Qt3DCore::QEntity *QBoundingVolumeDebugPrivate::findRootEntity(Qt3DCore::QEntity *e)
{
    Qt3DCore::QEntity *tmp = nullptr;
    Qt3DCore::QEntity *parentEntity = nullptr;
    while (e && (tmp = e->parentEntity()) != nullptr) {
        parentEntity = tmp;
        e = parentEntity;
    }
    return parentEntity;
}

void QBoundingVolumeDebugPrivate::updateSubtree()
{
    Q_Q(QBoundingVolumeDebug);
    if (m_debugSubtree == nullptr) {
        m_debugSubtree = new Qt3DCore::QEntity();
        m_sphereMesh = new Qt3DRender::QSphereMesh();
        m_transform = new Qt3DCore::QTransform();
        m_material = new Qt3DRender::QPhongAlphaMaterial();
        m_layer = new Qt3DRender::QLayer();

        static_cast<QPhongAlphaMaterial *>(m_material)->setAlpha(0.3f);
        static_cast<QPhongAlphaMaterial *>(m_material)->setDiffuse(QColor::fromRgb(255, 153, 0));

        m_layer->setNames(QStringList() << QStringLiteral("debug"));

        m_debugSubtree->addComponent(m_sphereMesh);
        m_debugSubtree->addComponent(m_transform);
        m_debugSubtree->addComponent(m_material);
        m_debugSubtree->addComponent(m_layer);

        m_transform->setTranslation(m_bvCenter);
        m_transform->setScale(m_bvRadius * 2.0f);
        m_sphereMesh->setRadius(0.5f);
        m_sphereMesh->setRings(100);
        m_sphereMesh->setSlices(100);

        // Insert into scene
        if (q->entities().size() > 0) {
            Qt3DCore::QEntity *rootEntity = findRootEntity(q->entities().constFirst());
            m_debugSubtree->setParent(rootEntity ? rootEntity : q->entities().constFirst());
        }
    } else {
        // Just update the mesh
        m_transform->setTranslation(m_bvCenter);
        m_transform->setScale(m_bvRadius * 2.0f);
    }
}

} // Qt3DRender

QT_END_NAMESPACE

#endif
