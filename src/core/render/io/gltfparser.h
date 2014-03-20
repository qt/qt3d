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

#ifndef GLTFPARSER_H
#define GLTFPARSER_H

#include "meshdata.h"

#include <entity.h>
#include <technique.h>

#include <QJsonDocument>
#include <QMultiHash>
#include <QImage>

class QFile;

namespace Qt3D {

class Material;
class ShaderProgram;
class Effect;
class Camera;
class Texture;

class GLTFParser
{
public:
    GLTFParser();

    static bool isGLTFPath(const QString& path);

    void setFilePath(QString path);

    void setBasePath( QString path );

    bool setJSON( QJsonDocument json );

    /**
     * @brief instantiate Create Nodes based on glTf JSON document
     * @return A new scene-graph fragment based on the provided glTf
     */
    Entity *node(QString id);
    Entity *defaultScene();
    MeshDataPtr mesh(QString id);
    Entity *scene(QString id);
    Material *material(QString id);
    Camera *camera(QString id);

private:
    void parse();

    void processJSONMesh( QString id, QJsonObject jsonObj );
    void processJSONAccessor(QString id, const QJsonObject &json);
    void processJSONBuffer(QString id, const QJsonObject &json);
    void processJSONBufferView(QString id, const QJsonObject &json);

    void processName( const QJsonObject& json, QObject* ins );

    QJsonDocument m_json;
    QString m_basePath;
    bool m_parseDone;
    QString m_defaultScene;

    // multi-hash because our MeshData corresponds to a single primitive
    // in glTf.
    QMultiHash<QString, MeshDataPtr> m_meshDict;

    // GLTF assigns materials at the mesh level, but we do them as siblings,
    // so record the association here for when we instantiate meshes
    QMap<MeshData*, QString> m_meshMaterialDict;

    QMap<QString, AttributePtr> m_attributeDict;

    class BufferData
    {
    public:
        BufferData();

        BufferData(QJsonObject json);

        quint64 length;
        QString path;
        // type if ever useful
    };

    QMap<QString, Material*> m_materialCache;

    QMap<QString, BufferData> m_bufferDatas;
    QMap<QString, BufferPtr> m_buffers;

    QMap<QString, QString> m_shaderPaths;
    QMap<QString, ShaderProgram*> m_programs;

    QMap<QString, Technique *> m_techniques;
    // glTF doesn't deal in effects, but we need a trivial one to wrap
    // up our techniques
    QMap<QString, Effect*> m_effectProxies;

    QMap<QString, Texture*> m_textures;
    QMap<QString, QImage> m_images;

    QFile* resolveLocalData(QString path);

    void processJSONShader(QString id, QJsonObject jsonObj);
    void processJSONProgram(QString id, QJsonObject jsonObj);
    void processJSONTechnique(QString id, QJsonObject jsonObj);

    void processJSONImage(QString id, QJsonObject jsonObj);
    void processJSONTexture(QString id, QJsonObject jsonObj);

    QVariant parameterValueFromJSON(Parameter *p, QJsonValue val);

    DrawState *buildState(const QByteArray& nm, QJsonValue obj);
};

}

#endif // GLTFPARSER_H
