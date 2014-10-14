/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsceneloader.h"
#include "qabstractsceneloader_p.h"
#include <Qt3DCore/qsceneinterface.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qentity.h>
#include "renderlogging.h"
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QSceneLoader::QSceneLoader(QNode *parent)
    : Render::QAbstractSceneLoader(parent)
{
}

// Called in main thread
void QSceneLoader::sceneChangeEvent(const QSceneChangePtr &change)
{
    Q_D(Render::QAbstractSceneLoader);
    QScenePropertyChangePtr e = qSharedPointerCast<QScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("scene")) {
            QEntity *scene = e->value().value<QEntity *>();
            // TO DO: We should send a QNodePtr so that it is release automatically
            if (scene != Q_NULLPTR && d->m_scene != Q_NULLPTR) {
                QList<QUuid> entities = d->m_scene->entitiesForComponent(d->m_uuid);
                if (entities.size() > 1) // TO DO: QComponent shareable property
                    qCWarning(Render::Frontend) << "It is strongly discouraged to share SceneLoader component between entities";
                Q_FOREACH (const QUuid &id, entities) {
                    QEntity *parentEntity = qobject_cast<QEntity *>(d->m_scene->lookupNode(id));
                    if (parentEntity != Q_NULLPTR) {
                        QEntity *cloneScene = qobject_cast<QEntity *>(QNodePrivate::get(scene)->clone());
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

QNode *QSceneLoader::doClone() const
{
    QSceneLoader *clone = new QSceneLoader();
    clone->d_func()->copy(d_func());
    return clone;
}

} // Qt3D

QT_END_NAMESPACE
