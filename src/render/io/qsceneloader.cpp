/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qsceneloader.h"
#include "qsceneloader_p.h"
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QSceneLoaderPrivate::QSceneLoaderPrivate()
    : QComponentPrivate()
    , m_status(QSceneLoader::Loading)
{
    m_shareable = false;
}


QSceneLoader::QSceneLoader(QNode *parent)
    : Qt3DCore::QComponent(*new QSceneLoaderPrivate, parent)
{
}

QSceneLoader::QSceneLoader(QSceneLoaderPrivate &dd, QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

QSceneLoader::~QSceneLoader()
{
    QNode::cleanup();
}

// Called in main thread
void QSceneLoader::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QSceneLoader);
    QScenePropertyChangePtr e = qSharedPointerCast<QScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("scene")) {
            // We receive a QNodePtr so that it is released automatically
            QNodePtr nodePtr = e->value().value<QNodePtr>();
            QEntity *scene = static_cast<QEntity *>(nodePtr.data());
            if (scene != Q_NULLPTR && d->m_scene != Q_NULLPTR) {
                QVector<QNodeId> entities = d->m_scene->entitiesForComponent(d->m_id);
                if (entities.size() > 1) // TO DO: QComponent shareable property
                    qCWarning(Render::Frontend) << "It is strongly discouraged to share SceneLoader component between entities";
                Q_FOREACH (QNodeId id, entities) {
                    QEntity *parentEntity = qobject_cast<QEntity *>(d->m_scene->lookupNode(id));
                    if (parentEntity != Q_NULLPTR) {
                        QEntity *cloneScene = qobject_cast<QEntity *>(QNode::clone(scene));
                        QNodePrivate::get(parentEntity)->insertTree(cloneScene);
                    }
                }
            }
        }
        else if (e->propertyName() == QByteArrayLiteral("status")) {
            setStatus(static_cast<QSceneLoader::Status>(e->value().toInt()));
        }
    }
}

void QSceneLoader::copy(const QNode *ref)
{
    const QSceneLoader *s = static_cast<const QSceneLoader*>(ref);
    d_func()->m_source = s->d_func()->m_source;
}

QUrl QSceneLoader::source() const
{
    Q_D(const QSceneLoader);
    return d->m_source;
}

void QSceneLoader::setSource(const QUrl &arg)
{
    Q_D(QSceneLoader);
    if (d->m_source != arg) {
        d->m_source = arg;
        emit sourceChanged(arg);
    }
}

QSceneLoader::Status QSceneLoader::status() const
{
    Q_D(const QSceneLoader);
    return d->m_status;
}

void QSceneLoader::setStatus(QSceneLoader::Status status)
{
    Q_D(QSceneLoader);
    if (d->m_status != status) {
        d->m_status = status;
        emit statusChanged(status);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QSceneLoader::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QSceneLoaderData>::create(this);
    auto &data = creationChange->data;
    data.source = d_func()->m_source;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
