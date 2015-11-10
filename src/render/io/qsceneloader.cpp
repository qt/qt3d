/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qsceneloader.h"
#include "qabstractsceneloader_p.h"
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QSceneLoader::QSceneLoader(QNode *parent)
    : QAbstractSceneLoader(parent)
{
}

QSceneLoader::~QSceneLoader()
{
    QNode::cleanup();
}

// Called in main thread
void QSceneLoader::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    QAbstractSceneLoaderPrivate *d = static_cast<QAbstractSceneLoaderPrivate*>(QNodePrivate::get(this));
    QScenePropertyChangePtr e = qSharedPointerCast<QScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("scene")) {
            // We receive a QNodePtr so that it is released automatically
            QNodePtr nodePtr = e->value().value<QNodePtr>();
            QEntity *scene = static_cast<QEntity *>(nodePtr.data());
            if (scene != Q_NULLPTR && d->m_scene != Q_NULLPTR) {
                QList<QNodeId> entities = d->m_scene->entitiesForComponent(d->m_id);
                if (entities.size() > 1) // TO DO: QComponent shareable property
                    qCWarning(Render::Frontend) << "It is strongly discouraged to share SceneLoader component between entities";
                Q_FOREACH (const QNodeId &id, entities) {
                    QEntity *parentEntity = qobject_cast<QEntity *>(d->m_scene->lookupNode(id));
                    if (parentEntity != Q_NULLPTR) {
                        QEntity *cloneScene = qobject_cast<QEntity *>(QNode::clone(scene));
                        QNodePrivate::get(parentEntity)->insertTree(cloneScene);
                    }
                }
            }
        }
        else if (e->propertyName() == QByteArrayLiteral("status")) {
            QAbstractSceneLoader::setStatus(static_cast<QAbstractSceneLoader::Status>(e->value().toInt()));
        }
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
