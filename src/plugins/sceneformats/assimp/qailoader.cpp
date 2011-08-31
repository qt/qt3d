/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

#include "qailoader.h"
#include "qaiscenehandler.h"
#include "qaiscene.h"
#include "qaimesh.h"

#include "qgeometrydata.h"
#include "qgltwosidedmaterial.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qgltexture2d.h"
#include "qglscenenode.h"
#include "qlogicalvertex.h"

#include "aiScene.h"
#include "aiMaterial.h"
#include "aiMesh.h"
#include "DefaultLogger.h"

#include <QtCore/qdir.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileinfo.h>

QAiLoader::QAiLoader(const aiScene *scene, QAiSceneHandler* handler)
     : m_scene(scene)
     , m_root(0)
     , m_handler(handler)
     , m_hasTextures(false)
     , m_hasLitMaterials(false)
     , m_builder(new QGLMaterialCollection(m_root))
{
    //nothing to do here
}

QAiLoader::~QAiLoader()
{
    // nothing to do here - m_rootNode is taken ownership of by caller of
    // rootNode() method
}

static inline void assertOnePrimitiveType(aiMesh *mesh)
{
#ifndef QT_NO_DEBUG
    int k = 0;  // count the number of bits set in the primitives
    unsigned int msk = 0x01;
    for (unsigned int p = mesh->mPrimitiveTypes; p; p >>= 1)
        if (p & msk)
            ++k;
    Q_ASSERT(k == 1);  // Assimp SortByPType promises this
#else
    Q_UNUSED(mesh);
#endif
}

static inline bool qHasTextures(const QGLSceneNode *node)
{
    QGLMaterial *mat = node->material();
    for (int i = 0; i < mat->textureLayerCount(); ++i)
        if (node->material()->texture(i))
            return true;
    return false;
}

void QAiLoader::loadMesh(aiMesh *mesh)
{
    QString name = QString::fromUtf8(mesh->mName.data, mesh->mName.length);
    // qDebug() << "loadMesh" << name << "with" << mesh->mNumVertices << "vertices"
    //            << "and" << mesh->mNumFaces << "faces";

    assertOnePrimitiveType(mesh);

    if (mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
    {
        m_builder.newSection();
        QGLSceneNode *node = m_builder.currentNode();
        node->setObjectName(name);
        QAiMesh m(mesh);
        m.build(m_builder, m_handler->showWarnings());
        m_meshes.append(node);
        if (qHasTextures(node))
            m_hasTextures = true;
        else
            m_hasLitMaterials = true;
    }
    else
    {
        // TODO: Implement other types in qaimesh.cpp
        if (m_handler->showWarnings())
        {
            QString error = QLatin1String("Bad primitive type in mesh %1 : %2");
            error = error.arg(name).arg(mesh->mPrimitiveTypes);
            Assimp::DefaultLogger::get()->warn(error.toStdString());
        }
    }

    if (m_handler->showWarnings())
    {
        if (!(m_handler->removeComponentFlags() & aiComponent_COLORS))
        {
            // TODO:  Implement models with per vertex colors.  Ok, the
            // vertex colors were wanted but are not supported yet.
            for (int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i)
            {
                if (mesh->HasVertexColors(i))
                {
                    QString error = QLatin1String(
                                "Found color information in mesh %1, channel %2"
                                "- per vertex color not yet supported");
                    error = error.arg(name).arg(i);
                    Assimp::DefaultLogger::get()->warn(error.toStdString());
                    break;
                }
            }
        }
        if (mesh->HasBones())
        {
            // TODO: Implement skeletal animation
            QString error = QLatin1String("Bones in mesh %1 not yet supported");
            error.arg(name);
            Assimp::DefaultLogger::get()->warn(error.toStdString());
        }
        if (mesh->HasTangentsAndBitangents())
        {
            // TODO: Implement normal maps - here and in the texture import
            QString error = QLatin1String("Tangents for normal map in mesh %1 not yet supported");
            error.arg(name);
            Assimp::DefaultLogger::get()->warn(error.toStdString());
        }
    }
}

inline static QMatrix4x4 getNodeMatrix(aiNode *node)
{
    QMatrix4x4 nodeMatrix;
    if (node->mTransformation.IsIdentity())
        return nodeMatrix;
    aiQuaternion rotation;
    aiVector3D position;
    aiVector3D scale;
    node->mTransformation.Decompose(scale, rotation, position);
    QVector3D qscale(scale.x,scale.y, scale.z);
    QVector3D qposition(position.x, position.y, position.z);
    QQuaternion qrotation(rotation.w, rotation.x, rotation.y, rotation.z);
    if (!qscale.isNull())
        nodeMatrix.scale(qscale);
    if (!qposition.isNull())
        nodeMatrix.translate(qposition);
    if (!qrotation.isNull())
        nodeMatrix.rotate(qrotation);
    return nodeMatrix;
}

void QAiLoader::loadNodes(aiNode *nodeList, QGLSceneNode *parentNode)
{
    QMap<aiNode *, QGLSceneNode *>::const_iterator it = m_nodeMap.constFind(nodeList);
    QGLSceneNode *node = 0;
    if (it == m_nodeMap.constEnd()) // not found
    {
        node = new QGLSceneNode(parentNode);
        m_nodes.append(node);
        QString name = QString::fromUtf8(nodeList->mName.data, nodeList->mName.length);
        if (name.isEmpty())
            name = QString(QLatin1String("aiNode %1")).arg(m_nodes.size());
        node->setObjectName(name);
        QMatrix4x4 mat = getNodeMatrix(nodeList);
        if (!mat.isIdentity())
            node->setLocalTransform(mat);
        for (unsigned int i = 0; i < nodeList->mNumChildren; ++i)
            loadNodes(nodeList->mChildren[i], node);
        for (unsigned int i = 0; i < nodeList->mNumMeshes; ++i)
        {
            int n = nodeList->mMeshes[i];
            if (n < m_meshes.size())
                node->addNode(m_meshes.at(n));
        }
    }
    else
    {
        node = it.value();
        parentNode->addNode(node);
    }
}

/*!
    \internal
    Loads all the geometry, materials, and texture associations from the assigned
    file, and returns the root node of the resulting scene graph.

    The caller must take ownership of the root node returned, and delete it
    when its no longer required.
*/
QGLSceneNode *QAiLoader::loadMeshes()
{
    Q_ASSERT(m_scene);
    for (unsigned int i = 0; i < m_scene->mNumMaterials; ++i)
        loadMaterial(m_scene->mMaterials[i]);

    // builds a naive scene heierarchy with all meshes under the root node
    for (unsigned int i = 0; i < m_scene->mNumMeshes; ++i)
        loadMesh(m_scene->mMeshes[i]);

    // fetch the naive scene heierarchy from the builder
    m_root = m_builder.finalizedSceneNode();

    QString name = m_handler->url().path();
    int pos = name.lastIndexOf("/");
    if (pos == -1)
        pos = name.lastIndexOf("\\");
    if (pos != -1)
        name = name.mid(pos+1);
    m_root->setObjectName(name);

    // if scene has a node heierarchy replace the naive heierarchy with that
    if (m_scene->mRootNode->mNumChildren > 0 && m_scene->mRootNode->mChildren)
    {
        m_root->removeNodes(m_root->children());
        loadNodes(m_scene->mRootNode, m_root);
    }

    if (m_hasTextures) // make textures the default
    {
        m_root->setEffect(QGL::LitModulateTexture2D);
        if (m_hasLitMaterials)
        {
            for (int i = 0; i < m_meshes.size(); ++i)
                if (!qHasTextures(m_meshes.at(i)))
                    m_meshes.at(i)->setEffect(QGL::LitMaterial);
        }
    }
    else
    {
        m_root->setEffect(QGL::LitMaterial);
    }

    if (m_handler->showWarnings())
    {
        QString message = QLatin1String("AssetImporter loader %1 -- "
                                        "Mesh count: %2 -- Node count: %3 -- "
                                        "Material count: %4");
        QUrl url = m_handler->url();
        message = message.arg(url.toString()).arg(m_meshes.size())
                .arg(m_nodes.size()).arg(m_root->palette()->size());
        Assimp::DefaultLogger::get()->warn(message.toStdString());
    }

//#define DEBUG_ME
#ifdef DEBUG_ME
    qDumpScene(m_root);

    QList<QGLSceneNode*> c = m_root->allChildren();
    QSet<quint64> debugged;
    for (int i = 0; i < c.size(); ++i)
    {
        if (c.at(i)->geometry().count() > 0)
        {
            QGeometryData g = c.at(i)->geometry();
            qDebug() << "geometry for:" << c.at(i) << "is:" << g.id();
            if (!debugged.contains(g.id()))
            {
                qDebug() << g;
                debugged.insert(g.id());
            }
        }
    }
#endif

    return m_root;
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
QUrl QAiLoader::ensureResource(const QString &path)
{
    QUrl res;
    QUrl base = m_handler->url();
    if (base.scheme() == QLatin1String("file"))
    {
        res = base.resolved(path);
        //qDebug() << "ensureResource - base:" << base
        //         << " -- path:" << path << "-- resolved:" << res;
        if (QFile::exists(res.path())) // shortcut common case
            return res;
        QStringList paths;
        paths << QLatin1String(".") << QLatin1String(":/");    // current directory and aliased/root resource file
        if (!base.isEmpty())
        {
            QFileInfo fi(base.path());
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
        res = base.resolved(path);
    }
    return res;
}

void QAiLoader::loadTextures(aiMaterial *ma, QGLMaterial *mq)
{
    int texCount;
    aiTextureType texType;
    if (m_handler->showWarnings())
    {
        // TODO: AssImp has a very rich set of texture related functionality
        // but until things in this list get implemented, all are just going
        // to get ignored, tho' they will generate a warning in verbose mode.
        // So for now, its just inside this conditional - move it out once
        // implementation of these items begins.
        static const char * typeNames[] = {
            "None",
            "Diffuse",
            "Specular",
            "Ambient",
            "Emissive",
            "Height",
            "Normals",
            "Shininess",
            "Opacity",
            "Displacement",
            "Lightmap",
            "Reflection",
            "Unknown",
            0
        };

        for (unsigned int i = 0; i <= aiTextureType_UNKNOWN; ++i)
        {
            texType = static_cast<aiTextureType>(i);
            texCount = ma->GetTextureCount(texType);
            if (texCount && texType != aiTextureType_DIFFUSE)
            {
                QString error = QLatin1String("Unsupported texture type \"%1\" in material \"%2\".");
                error.arg(typeNames[i]).arg(mq->objectName());
                Assimp::DefaultLogger::get()->warn(error.toStdString());
            }
        }
    }
    // TODO: For now assume the only texture we care about is the diffuse one
    //
    texCount = ma->GetTextureCount(aiTextureType_DIFFUSE);
    if (texCount > 0)
    {
        if (texCount > 1 && m_handler->showWarnings())
        {
            QString error = QLatin1String("Multi-textures not supported: \"%1\" has %2");
            error.arg(mq->objectName()).arg(texCount);
            Assimp::DefaultLogger::get()->warn(error.toStdString());
        }
        else
        {
            aiString path;
            ma->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path);
            QString qpath = QString::fromUtf8(path.data, path.length);
            QUrl url = ensureResource(qpath);
            if (url.isEmpty())
            {
                if (m_handler->showWarnings())
                {
                    QString error = QLatin1String("Could not load texture: %1 for material %2");
                    error.arg(url.toString()).arg(mq->objectName());
                    Assimp::DefaultLogger::get()->warn(error.toStdString());
                }
            }
            else
            {
                mq->setTextureUrl(url);
            }
        }
    }
}

/*!
  Load a material
*/
void QAiLoader::loadMaterial(aiMaterial *ma)
{
    QGLMaterialCollection *palette = m_builder.palette();
    QGLMaterial *mq = new QGLMaterial;
    mq->setObjectName("___DEFAULT_NAME___");

    bool isTwoSided = false;
    bool isWireframe = false;
    struct aiColor4D clr;
    float shininess;
    float amplitude;

    aiReturn r;

    // In AssImp if the material is two sided then it means cull face
    // should be turned off: http://assimp.sourceforge.net/lib_html/materials.html
    r = ma->Get(AI_MATKEY_TWOSIDED, isTwoSided);
    if (r == aiReturn_SUCCESS && isTwoSided)
        mq->setProperty("isTwoSided", isTwoSided);

    aiString aName;
    r = ma->Get(AI_MATKEY_NAME, aName);
    if (r == aiReturn_SUCCESS && aName.length > 0)
        mq->setObjectName(QLatin1String(aName.data));

    r = ma->Get(AI_MATKEY_COLOR_AMBIENT, clr);
    if (r == aiReturn_SUCCESS)
        mq->setAmbientColor(QColor::fromRgbF(clr.r, clr.g, clr.b, clr.a));

    r = ma->Get(AI_MATKEY_COLOR_DIFFUSE, clr);
    if (r == aiReturn_SUCCESS)
        mq->setDiffuseColor(QColor::fromRgbF(clr.r, clr.g, clr.b, clr.a));

    r = ma->Get(AI_MATKEY_COLOR_SPECULAR, clr);
    if (r == aiReturn_SUCCESS)
    {
        QColor spec = QColor::fromRgbF(clr.r, clr.g, clr.b, clr.a);
        mq->setSpecularColor(spec);

        // By default the specular color is black - very dark.
        // If the specular color is bright  be careful with shininess -
        // a shininess of 0 will blow everything out to full white.  So in
        // the case of bad materials with this problem, set shiness low.
        mq->setShininess(64.0);
    }

    r = ma->Get(AI_MATKEY_SHININESS, shininess);
    if (r == aiReturn_SUCCESS)
        mq->setShininess(shininess);

    r = ma->Get(AI_MATKEY_SHININESS_STRENGTH, amplitude);
    if (r == aiReturn_SUCCESS)
        mq->setShininess(shininess * amplitude);

    r = ma->Get(AI_MATKEY_ENABLE_WIREFRAME, isWireframe);
    if (r == aiReturn_SUCCESS && isWireframe)
        mq->setProperty("isWireFrame", isWireframe);

    loadTextures(ma, mq);

    // INVARIANT: since we create the palette newly in this class, and this
    // function is the only place we add materials to this palette, the index
    // values (the positions in the palette generated by this call to addMaterial)
    // will exactly match the index values of the materials traversed in the
    // for loop in loadMeshes() - so therefore AI's index values and the ones in
    // the palette will be the same.
    //
    // executive summary: don't muck around with the palettte outside of this call

    int k = palette->addMaterial(mq);

    Q_UNUSED(k);
    //qDebug() << "loaded material" << k << mq;
}
