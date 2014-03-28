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

#include "scene.h"

#include <QDebug>

#include <gltfparser.h>
#include <assimpparser.h>

namespace Qt3D
{

Scene::Scene(Node* parent) :
    Entity(parent),
    m_sceneChild(Q_NULLPTR)
{
}

QString Scene::source() const
{
    return m_source;
}

QString Scene::sceneId() const
{
    return m_sceneId;
}

Scene *Scene::findInTree(Node *root)
{
    if (!root)
        return Q_NULLPTR;

    Scene* s = qobject_cast<Scene*>(root);
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

void Scene::clear()
{
    removeAllChildren();
}

void Scene::setSource(QString arg)
{
    if (m_source != arg) {
        m_source = arg;
        rebuild();
        emit sourceChanged(arg);
    }
}

void Qt3D::Scene::setSceneId(QString arg)
{
    if (m_sceneId != arg) {
        m_sceneId = arg;
        rebuild();
        emit sceneIdChanged(arg);
    }
}

void Scene::rebuild()
{
    if (m_sceneChild != Q_NULLPTR) {
        removeChild(m_sceneChild);
        m_sceneChild->deleteLater();
    }
    // Maybe move scene parsers to plugins
    // And handle priority if a format is handled by more than one parser
    if (GLTFParser::isGLTFPath(m_source)) {
        qDebug() << Q_FUNC_INFO << "will load GLTF scene";

        GLTFParser parser;
        parser.setFilePath(m_source);

        if (!m_sceneId.isEmpty())
            m_sceneChild = parser.scene(m_sceneId);
        else
            m_sceneChild = parser.defaultScene();

        if (m_sceneChild != Q_NULLPTR) {
            addChild(m_sceneChild);
        }
    }
    else if (AssimpParser::isAssimpPath(m_source)) {
        qDebug() << Q_FUNC_INFO << "will load Assimp scene";
        AssimpParser parser;
        parser.setFilePath(m_source);
        m_sceneChild = parser.scene(m_sceneId);
        if (m_sceneChild) {
            qDebug() << Q_FUNC_INFO << "Assimp scene not empty";
            addChild(m_sceneChild);
        }
    }
    else {
        qWarning() << Q_FUNC_INFO << "Scene file format not supported by Qt3D";
    }
}

} // of namespace
