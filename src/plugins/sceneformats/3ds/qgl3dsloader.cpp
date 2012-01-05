/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgl3dsloader.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qgltexture2d.h"
#include "qgl3dsscene.h"
#include "qglscenenode.h"
#include "qgl3dsmesh.h"

#include <lib3ds/mesh.h>
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>

#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdir.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileinfo.h>

QGL3dsLoader::QGL3dsLoader(Lib3dsFile *file, QGL3dsSceneHandler* sh)
     : m_file(file)
     , m_rootNode(new QGLSceneNode())
     , m_scene(sh)
     , m_hasTextures(false)
{
    m_rootNode->setPalette(new QGLMaterialCollection(m_rootNode));
    m_rootNode->setObjectName(QString::fromLocal8Bit(file->name));
    setUrl(sh->url());
}

QGL3dsLoader::~QGL3dsLoader()
{
    // nothing to do here - m_rootNode is taken ownership of by caller of
    // rootNode() method
}

/*!
    Sets the location for resolving resource paths to \a url.

    Only URL's with a "file" scheme are supported.
*/
void QGL3dsLoader::setUrl(const QUrl &url)
{
    // TODO - handling of network URLs
    if (url.scheme() == QLatin1String("file"))
        m_url = url;
    else
        qWarning("QGL3dsLoader::setUrl(%s): ignored, only file urls supported",
                 url.encodedPath().constData());
}

void QGL3dsLoader::loadMesh(Lib3dsMesh *mesh)
{
#ifndef QT_NO_DEBUG_STREAM
    if (mesh->points == 0 && (m_scene->options() & QGL::ShowWarnings))
        qDebug() << "Mesh" << mesh->name << "has zero vertex count";
    else if (mesh->faces == 0 && (m_scene->options() & QGL::ShowWarnings))
        qDebug() << "Mesh" << mesh->name << "has zero face count";
#endif
    QGL3dsMesh m(mesh, m_rootNode->palette());
    QString name = QString::fromLatin1(mesh->name);
    QGL::ModelOptions o = m_scene->meshOptions(name);
    if (o == 0)
        o = m_scene->options();
    m.setOptions(o);
    QGLSceneNode *node;
    if (mesh->faces == 0 || mesh->points == 0)
    {
        node = new QGLSceneNode(m_rootNode);
        node->setObjectName(name);
        m_meshes.insert(name, node);
        return;
    }
    m.initialize();
    if (!m_hasTextures)
        m_hasTextures = m.hasTexture();
    if (!m_hasLitMaterials)
        m_hasLitMaterials = !m.hasTexture();
    node = m.finalizedSceneNode();
    m_meshes.insert(name, node);
    m_rootNode->addNode(node);
}

#ifndef QT_NO_DEBUG_STREAM
static const char *node_type_names[] = {
    "LIB3DS_UNKNOWN_NODE",
    "LIB3DS_AMBIENT_NODE",
    "LIB3DS_OBJECT_NODE",
    "LIB3DS_CAMERA_NODE",
    "LIB3DS_TARGET_NODE",
    "LIB3DS_LIGHT_NODE",
    "LIB3DS_SPOT_NODE"
};
#endif

inline static QMatrix4x4 getNodeMatrix(Lib3dsNode *node)
{
    QMatrix4x4 nodeMatrix;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
           nodeMatrix(row, col) = node->matrix[col][row];
    nodeMatrix.optimize();
    Lib3dsObjectData *d = &node->data.object;
    if (!qFuzzyIsNull(d->pivot[0]) || !qFuzzyIsNull(d->pivot[1]) || !qFuzzyIsNull(d->pivot[2]))
        nodeMatrix.translate(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
    return nodeMatrix;
}

void QGL3dsLoader::loadNodes(Lib3dsNode *nodeList, QGLSceneNode *parentNode)
{
    Lib3dsNode *node;
    for (node = nodeList; node != NULL; node = node->next)
    {
        if (node->type == LIB3DS_OBJECT_NODE)
        {
            Lib3dsObjectData *d = &node->data.object;
            QString meshName = QString::fromLatin1(d->morph);
            if (meshName.isEmpty())
                meshName = QString::fromLatin1(d->instance);
            if (meshName.isEmpty())
                meshName = QString::fromLatin1(node->name);
            if (!meshName.isEmpty() && m_meshes.contains(meshName))
            {
                QGLSceneNode *mesh = m_meshes[meshName];
                QMatrix4x4 mat = getNodeMatrix(node);
                mat = mat * mesh->localTransform();
                mesh->setLocalTransform(mat);
            }
            else
            {
                QGLSceneNode *sceneNode = new QGLSceneNode(parentNode);
                sceneNode->setPalette(parentNode->palette());
                sceneNode->setLocalTransform(getNodeMatrix(node));
                //sceneNode->userTransform().setToIdentity();  //DP: set matrix to identity so it is initialised in a useful way at least.
                QString nodeName(QString::fromLatin1(node->name));
                if (nodeName == QLatin1String("$$$DUMMY"))
                {
                    nodeName = QString::fromLatin1(node->data.object.instance);
                    sceneNode->setObjectName(nodeName);
                    loadNodes(node->childs, sceneNode);
                }
            }
        }
#ifndef QT_NO_DEBUG_STREAM
        else
        {
            qDebug() << "Node" << node->name << "of type" << node_type_names[node->type] << "not currently supported";
        }
#endif
    }
}

/*!
    \internal
    Loads all the geometry, materials, and texture associations from the assigned
    file, and returns the root node of the resulting scene graph.

    The caller must take ownership of the root node returned, and delete it
    when its no longer required.
*/
QGLSceneNode *QGL3dsLoader::loadMeshes()
{
    Q_CHECK_PTR(m_file);
    Lib3dsMaterial *mat;
    for (mat = m_file->materials; mat != NULL; mat = mat->next)
        loadMaterial(mat);
    Lib3dsMesh * mesh;
    for (mesh = m_file->meshes; mesh != NULL; mesh = mesh->next)
        loadMesh(mesh);
    QGL::MeshOptionMap optList = m_scene->meshOptions();
    QStringList optionedMeshes = optList.keys();
    QStringList gotMeshes = m_meshes.keys();
    for (int i = 0; i < gotMeshes.size(); ++i)
        optionedMeshes.removeAll(gotMeshes.at(i));
    for (int i = 0; i < optionedMeshes.size(); ++i)
        qWarning("Option specified, but mesh %s not found",
                 qPrintable(optionedMeshes.at(i)));
    m_rootNode->palette()->removeUnusedMaterials();
    loadNodes(m_file->nodes, m_rootNode);
    m_rootNode->setEffect(m_hasTextures ? QGL::LitModulateTexture2D : QGL::LitMaterial);
    return m_rootNode;
}

/*!
    \internal
    Search for a resource based on the given \a path.

    If the URL for the currently loading mesh has a scheme other than
    "file" then a URL with the path relative to that URL is returned.

    If the URL for the currently loading mesh has a "file" scheme, then
    first a case-sensitive search is done of all of the current directory,
    and the :/ resource directory, and the directory of the current mesh
    file.

    If the file is not found in any of those locations then they are
    searched again case-insensitively.  If the file is found, then a
    URL based on the absolute file path of the matching file is returned.

    Otherwise an empty string is returned.
*/
QUrl QGL3dsLoader::ensureResource(const QString &path)
{
    QUrl res;
    if (m_url.scheme() == QLatin1String("file"))
    {
        res = m_url.resolved(path);
        if (QFile::exists(res.path())) // shortcut common case
            return res;
        QStringList paths;
        paths << QLatin1String(".") << QLatin1String(":/");    // current directory and aliased/root resource file
        if (!m_url.isEmpty())
        {
            QFileInfo fi(m_url.path());
            paths.prepend(fi.absoluteDir().absolutePath());
        }
        bool caseInsensitive = false;
        do {
            QStringList::const_iterator it(paths.begin());
            for ( ; it != paths.end(); ++it)
            {
                QDir resDir(*it);
                QStringList fileList = resDir.entryList(QDir::Files);
                if (caseInsensitive)
                {
                    QStringList::const_iterator fit(fileList.begin());
                    for ( ; fit != fileList.end(); ++fit)
                    {
                        if (fit->toLower() == path.toLower())
                        {
                            res.setScheme(QLatin1String("file"));
                            res.setPath(resDir.absoluteFilePath(*fit));
                            break;
                        }
                    }
                }
                else
                {
                    if (fileList.contains(path))
                    {
                        //return resDir.absoluteFilePath(path);
                        res.setScheme(QLatin1String("file"));
                        res.setPath(resDir.absoluteFilePath(path));
                        break;
                    }
                }
            }
            if (caseInsensitive)
                break;
            caseInsensitive = true;
        } while (true);
    }
    else
    {
        // non-file url
        res = m_url.resolved(path);
    }
    return res;
}

/*!
  Load a material
*/
void QGL3dsLoader::loadMaterial(Lib3dsMaterial *mat3ds)
{
    QGLMaterialCollection *palette = m_rootNode->palette();
    QGLMaterial *mat = new QGLMaterial();
    Lib3dsRgba &amb = mat3ds->ambient;
    Lib3dsRgba &dif = mat3ds->diffuse;
    Lib3dsRgba &spc = mat3ds->specular;
    mat->setAmbientColor(QColor::fromRgbF(amb[0], amb[1], amb[2], amb[3]));
    mat->setDiffuseColor(QColor::fromRgbF(dif[0], dif[1], dif[2], dif[3]));
    mat->setSpecularColor(QColor::fromRgbF(spc[0], spc[1], spc[2], spc[3]));
    mat->setShininess(128 * mat3ds->shininess);
    mat->setObjectName(QString::fromLatin1(mat3ds->name));
    palette->addMaterial(mat);
    if (mat3ds->texture1_map.name[0])
    {
        QString txName(QString::fromLatin1(mat3ds->texture1_map.name));
        QUrl url = ensureResource(txName);
        if (url.isEmpty())
        {
            if (m_scene->options() & QGL::ShowWarnings)
                qWarning("Could not load texture: %s", mat3ds->texture1_map.name);
        }
        else
        {
            mat->setTextureUrl(url);
        }
    }
}
