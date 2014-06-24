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

#ifndef ASSIMPPARSER_H
#define ASSIMPPARSER_H

// ASSIMP LIBRARY INCLUDE
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <Qt3DRenderer/meshdata.h>
#include <Qt3DRenderer/assimphelpers.h>
#include <Qt3DRenderer/abstractsceneparser.h>

#include <QMap>
#include <QDir>

QT_BEGIN_NAMESPACE

class QFile;

namespace Qt3D {

class Material;
class ShaderProgram;
class Effect;
class Camera;
class Texture;
class Mesh;

class AssimpParser : public AbstractSceneParser
{
    Q_OBJECT
public:
    AssimpParser();
    ~AssimpParser();

    static bool isAssimpPath(const QString& path);

    // SceneParserInterface interface
    void setFilePath(const QString& path) Q_DECL_OVERRIDE;
    bool isPathExtensionSupported(const QString &path) Q_DECL_OVERRIDE;
    Entity *scene(QString id = QString()) Q_DECL_OVERRIDE;
    Entity *node(QString id) Q_DECL_OVERRIDE;

    Entity *defaultScene();
    MeshDataPtr mesh(QString id);
    Material *material(QString id);
    Camera *camera(QString id);

private :
    static QStringList assimpSupportedFormats();
    static QMatrix4x4 aiMatrix4x4ToQMatrix4x4(const aiMatrix4x4 &matrix);

    Entity *node(aiNode *node);

    void readSceneFile(const QString &file);

    void cleanup();
    void parse();

    void loadMaterial(uint materialIndex);
    void loadMesh(uint meshIndex);
    void loadEmbeddedTexture(uint textureIndex);
    void loadLight(uint lightIndex);
    void loadCamera(uint cameraIndex);
    void loadAnimation(uint animationIndex);

    void copyMaterialName(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialColorProperties(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialFloatProperties(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialBoolProperties(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialShadingModel(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialBlendingFunction(Material *material, aiMaterial *assimpMaterial);
    void copyMaterialTextures(Material *material, aiMaterial *assimpMaterial);

    QScopedPointer<aiScene> m_aiScene;
    QDir     m_sceneDir;
    bool     m_sceneParsed;
    static QStringList assimpSupportedFormatsList;

    QMap<uint, Mesh *> m_meshes;
    QMap<uint, Material*> m_materials;
    QMap<uint, Effect *> m_effects;
    QMap<uint, Texture*> m_embeddedTextures;
    QMap<QString, Texture*> m_materialTextures;
    QMap<aiNode*, Entity*> m_cameras;

    QHash<aiTextureType, QString> m_textureToParameterName;

//    QMap<aiNode*, Light*> m_lights;

};

}

QT_END_NAMESPACE

#endif // ASSIMPPARSER_H
