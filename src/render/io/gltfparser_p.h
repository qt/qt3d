/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLTFPARSER_H
#define GLTFPARSER_H

#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DRenderer/qattribute.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qbuffer.h>
#include <QJsonDocument>
#include <QMultiHash>
#include <Qt3DRenderer/qabstractmesh.h>
#include <QImage>

#include <Qt3DRenderer/private/abstractsceneparser_p.h>

QT_BEGIN_NAMESPACE

class QFile;

namespace Qt3D {

class QMaterial;
class QShaderProgram;
class QEffect;
class QCamera;
class QCameraLens;
class QAbstractTextureProvider;

class GLTFParser : public AbstractSceneParser
{
    Q_OBJECT

public:
    GLTFParser();

    void setBasePath(const QString& path);
    bool setJSON( QJsonDocument json );

    // SceneParserInterface interface
    void setSource(const QUrl &source) Q_DECL_OVERRIDE;
    bool isExtensionSupported(const QUrl &source) const Q_DECL_OVERRIDE;

    /**
     * @brief instantiate Create Nodes based on glTf JSON document
     * @return A new scene-graph fragment based on the provided glTf
     */
    QEntity *node(const QString &id) Q_DECL_OVERRIDE;
    QEntity *scene(const QString &id = QString()) Q_DECL_OVERRIDE;

private:
    static bool isGLTFPath(const QString &path);

    QEntity *defaultScene();
    QMeshDataPtr mesh(const QString &id);
    QMaterial *material(const QString &id);
    QCameraLens *camera(const QString &id);

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

    // multi-hash because our QMeshData corresponds to a single primitive
    // in glTf.
    QMultiHash<QString, QMeshDataPtr> m_meshDict;

    // GLTF assigns materials at the mesh level, but we do them as siblings,
    // so record the association here for when we instantiate meshes
    QMap<QMeshData*, QString> m_meshMaterialDict;

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

    QMap<QString, QMaterial*> m_materialCache;

    QMap<QString, BufferData> m_bufferDatas;
    QMap<QString, BufferPtr> m_buffers;

    QMap<QString, QString> m_shaderPaths;
    QMap<QString, QShaderProgram*> m_programs;

    QMap<QString, QTechnique *> m_techniques;
    // glTF doesn't deal in effects, but we need a trivial one to wrap
    // up our techniques
    QMap<QString, QEffect*> m_effectProxies;

    QMap<QString, QAbstractTextureProvider*> m_textures;
    QMap<QString, QImage> m_images;

    QFile* resolveLocalData(QString path);

    void processJSONShader(QString id, QJsonObject jsonObj);
    void processJSONProgram(QString id, QJsonObject jsonObj);
    void processJSONTechnique(QString id, QJsonObject jsonObj);

    void processJSONImage(QString id, QJsonObject jsonObj);
    void processJSONTexture(QString id, QJsonObject jsonObj);

    QVariant parameterValueFromJSON(QParameter *p, QJsonValue val);

    //Render::RenderState *buildState(const QByteArray& nm, QJsonValue obj);
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // GLTFPARSER_H
