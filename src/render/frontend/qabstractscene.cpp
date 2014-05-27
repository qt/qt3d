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

#include "renderlogging.h"

#include <gltfparser.h>
#include <assimpparser.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

QAbstractScene::QAbstractScene(Node *sceneNode)
    : d_ptr(new QAbstractScenePrivate(this))
{
    Q_D(QAbstractScene);
    d->m_sceneNode = sceneNode;
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

QAbstractScene *QAbstractScene::findInTree(Node *root)
{
    if (!root)
        return Q_NULLPTR;

    QAbstractScene* s = qobject_cast<QAbstractScene*>(root);
    if (s)
        return s;

    // recursive walk down the tree
    foreach (Node* nd, root->children()) {
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

Node *QAbstractScene::node(QString id)
{
    Q_D(QAbstractScene);
    if (d->m_currentParser)
        return d->m_currentParser->node(id);
    return Q_NULLPTR;
}

Node *QAbstractScene::scene(QString id)
{
    Q_D(QAbstractScene);
    if (d->m_currentParser)
        return d->m_currentParser->scene(id);
    return Q_NULLPTR;
}

void QAbstractScene::rebuild()
{
    Q_D(QAbstractScene);
    if (d->m_sceneChild != Q_NULLPTR) {
        d->m_sceneNode->removeChild(d->m_sceneChild);
        d->m_sceneChild->deleteLater();
    }

    // Make scene parsers plugins
    QList<AbstractSceneParser *> parsers;
    parsers << new GLTFParser() << new AssimpParser();

    // Maybe move scene parsers to plugins
    // And handle priority if a format is handled by more than one parser

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
