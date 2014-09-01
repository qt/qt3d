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

#include "qabstractscene.h"
#include "qabstractscene_p.h"
#include "renderlogging.h"
#include <Qt3DRenderer/private/abstractsceneparser_p.h>
#include <Qt3DRenderer/private/gltfparser_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

QAbstractScenePrivate::QAbstractScenePrivate(QAbstractScene *qq)
    : QComponentPrivate(qq)
    , m_sceneNode(Q_NULLPTR)
    , m_sceneChild(Q_NULLPTR)
    , m_currentParser(Q_NULLPTR)
{
}

QAbstractScene::QAbstractScene(QAbstractScenePrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QAbstractScene::QAbstractScene(QNode *parent)
    : QComponent(*new QAbstractScenePrivate(this), parent)
{
    Q_D(QAbstractScene);
    d->m_sceneNode = this;
}

void QAbstractScene::copy(const QNode *ref)
{
    Q_D(QAbstractScene);
    const QAbstractScene *s = qobject_cast<const QAbstractScene *>(ref);
    if (s != Q_NULLPTR) {
        d->m_sceneId = s->sceneId();
        d->m_source = s->source();
    }
}

QString QAbstractScene::source() const
{
    Q_D(const QAbstractScene);
    return d->m_source;
}

QString QAbstractScene::sceneId() const
{
    Q_D(const QAbstractScene);
    return d->m_sceneId;
}

QAbstractScene *QAbstractScene::findInTree(QNode *root)
{
    if (!root)
        return Q_NULLPTR;

    QAbstractScene* s = qobject_cast<QAbstractScene*>(root);
    if (s)
        return s;

    // recursive walk down the tree
    foreach (QNode* nd, root->children()) {
        s = findInTree(nd);
        if (s)
            return s;
    }

    return Q_NULLPTR;
}

void QAbstractScene::clear()
{
    Q_D(QAbstractScene);
    d->m_sceneNode->removeAllChildren();
}

void QAbstractScene::setSource(QString arg)
{
    Q_D(QAbstractScene);
    if (d->m_source != arg) {
        d->m_source = arg;
        rebuild();
        emit sourceChanged(arg);
        QScenePropertyChangePtr change(new QScenePropertyChange(ComponentUpdated, this));
        change->setPropertyName(QByteArrayLiteral("source"));
        change->setValue(d->m_source);
        notifyObservers(change);
    }
}

void QAbstractScene::setSceneId(QString arg)
{
    Q_D(QAbstractScene);
    if (d->m_sceneId != arg) {
        d->m_sceneId = arg;
        rebuild();
        emit sceneIdChanged(arg);
    }
}

QNode *QAbstractScene::node(QString id)
{
    Q_D(QAbstractScene);
    if (d->m_currentParser)
        return d->m_currentParser->node(id);
    return Q_NULLPTR;
}

QNode *QAbstractScene::scene(QString id)
{
    Q_D(QAbstractScene);
    if (d->m_currentParser)
        return d->m_currentParser->scene(id);
    return Q_NULLPTR;
}

// Move that into a jobs
void QAbstractScene::rebuild()
{
    Q_D(QAbstractScene);
    if (d->m_sceneChild != Q_NULLPTR) {
        d->m_sceneNode->removeChild(d->m_sceneChild);
        d->m_sceneChild->deleteLater();
    }

    // Make scene parsers plugins
    QList<AbstractSceneParser *> parsers;
    parsers << new GLTFParser();// << new AssimpParser();

    // TO DO: Load Scene Parsers

    bool parserFound = false;
    Q_FOREACH (AbstractSceneParser *parser, parsers) {
        if (parser->isPathExtensionSupported(d->m_source)) {
            if (d->m_currentParser != Q_NULLPTR && parser != d->m_currentParser) {
                delete d->m_currentParser;
                d->m_currentParser = parser;
            }
            qCDebug(Render::Io) << Q_FUNC_INFO << parser;
            parser->setFilePath(d->m_source);
            if ((d->m_sceneChild = parser->scene(d->m_sceneId)) != Q_NULLPTR)
                d->m_sceneNode->addChild(d->m_sceneChild);
            parserFound = true;
            break;
        }
    }
    if (!parserFound)
        qCWarning(Render::Frontend) << Q_FUNC_INFO << "Scene file format not supported by Qt3D";
}

} // Render

} // namespace Qt3D

QT_END_NAMESPACE
