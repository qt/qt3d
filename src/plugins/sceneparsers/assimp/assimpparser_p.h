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

#ifndef QT3D_ASSIMPPARSER_H
#define QT3D_ASSIMPPARSER_H

// ASSIMP LIBRARY INCLUDE
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <Qt3DCore/qabstractmesh.h>
#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DRenderer/private/abstractsceneparser_p.h>
#include "assimphelpers_p.h"

#include <QMap>
#include <QDir>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

class QFile;

namespace Qt3D {

class QMaterial;
class QShaderProgram;
class QEffect;
class QCamera;
class QTexture;
class QMesh;
class AssimpMesh;

Q_DECLARE_LOGGING_CATEGORY(AssimpParserLog)

class AssimpParser : public AbstractSceneParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt3DRenderer.AssimParser")
public:
    AssimpParser();
    ~AssimpParser();

    static bool isAssimpPath(const QString& path);

    // SceneParserInterface interface
    void setFilePath(const QString& path) Q_DECL_OVERRIDE;
    bool isPathExtensionSupported(const QString &path) Q_DECL_OVERRIDE;
    QEntity *scene(QString id = QString()) Q_DECL_OVERRIDE;
    QEntity *node(QString id) Q_DECL_OVERRIDE;

    QEntity *defaultScene();
    QMeshDataPtr mesh(QString id);
    QMaterial *material(QString id);
    QCamera *camera(QString id);

private :

    static QStringList assimpSupportedFormats();
    static QMatrix4x4 aiMatrix4x4ToQMatrix4x4(const aiMatrix4x4 &matrix);

    QEntity *node(aiNode *node);

    void readSceneFile(const QString &file);

    void cleanup();
    void parse();

    void loadMaterial(uint materialIndex);
    void loadMesh(uint meshIndex);
    void loadEmbeddedTexture(uint textureIndex);
    void loadLight(uint lightIndex);
    void loadCamera(uint cameraIndex);
    void loadAnimation(uint animationIndex);

    void copyMaterialName(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialColorProperties(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialFloatProperties(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialBoolProperties(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialShadingModel(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialBlendingFunction(QMaterial *material, aiMaterial *assimpMaterial);
    void copyMaterialTextures(QMaterial *material, aiMaterial *assimpMaterial);

    QScopedPointer<aiScene> m_aiScene;
    QDir     m_sceneDir;
    bool     m_sceneParsed;
    static QStringList assimpSupportedFormatsList;

    QMap<uint, AssimpMesh *> m_meshes;
    QMap<uint, QMaterial*> m_materials;
    QMap<uint, QEffect *> m_effects;
    QMap<uint, QTexture*> m_embeddedTextures;
    QMap<QString, QTexture*> m_materialTextures;
    QMap<aiNode*, QEntity*> m_cameras;

    QHash<aiTextureType, QString> m_textureToParameterName;

//    QMap<aiNode*, Light*> m_lights;

};

}

QT_END_NAMESPACE

#endif // QT3D_ASSIMPPARSER_H
