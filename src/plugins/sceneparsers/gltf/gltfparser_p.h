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

#include <QtCore/QJsonDocument>
#include <QtCore/QMultiHash>

#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>

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
class QRenderState;
class QTechnique;
class QParameter;
class QEntity;
class QGeometryRenderer;

Q_DECLARE_LOGGING_CATEGORY(GLTFParserLog)

class GLTFParser : public AbstractSceneParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt3DRenderer.GLTFParser")

public:
    GLTFParser();
    ~GLTFParser();

    void setBasePath(const QString& path);
    bool setJSON( const QJsonDocument &json );

    // SceneParserInterface interface
    void setSource(const QUrl &source) Q_DECL_FINAL;
    bool isExtensionSupported(const QUrl &source) const Q_DECL_FINAL;
    QEntity *node(const QString &id) Q_DECL_FINAL;
    QEntity *scene(const QString &id = QString()) Q_DECL_FINAL;

private:
    class BufferData
    {
    public:
        BufferData();

        BufferData(QJsonObject json);

        quint64 length;
        QString path;
        QByteArray *data;
        // type if ever useful
    };

    class ParameterData
    {
    public:
        ParameterData();
        ParameterData(QJsonObject json);

        QString semantic;
        int type;
    };

    class AccessorData
    {
    public:
        AccessorData();
        AccessorData(const QJsonObject& json);

        QString bufferViewName;
        QAttribute::DataType type;
        uint dataSize;
        int count;
        int offset;
        int stride;
    };

    static bool isGLTFPath(const QString &path);
    static void renameFromJson(const QJsonObject& json, QObject * const object );
    static QString standardUniformNamefromSemantic(const QString &semantic);
    static QString standardAttributeNameFromSemantic(const QString &semantic);
    static QParameter *parameterFromTechnique(QTechnique *technique, const QString &parameterName);

    QEntity *defaultScene();
    QMaterial *material(const QString &id);
    QCameraLens *camera(const QString &id) const;

    void parse();
    void cleanup();

    void processJSONBuffer(const QString &id, const QJsonObject &json);
    void processJSONBufferView(const QString &id, const QJsonObject &json);
    void processJSONShader(const QString &id, const QJsonObject &jsonObject);
    void processJSONProgram(const QString &id, const QJsonObject &jsonObject);
    void processJSONTechnique(const QString &id, const QJsonObject &jsonObject);
    void processJSONAccessor(const QString &id, const QJsonObject &json);
    void processJSONMesh(const QString &id, const QJsonObject &json);
    void processJSONImage(const QString &id, const QJsonObject &jsonObject);
    void processJSONTexture(const QString &id, const QJsonObject &jsonObject);

    void loadBufferData();
    void unloadBufferData();

    QFile* resolveLocalData(QString path) const;

    QVariant parameterValueFromJSON(int type, const QJsonValue &value) const;
    static QAbstractAttribute::DataType accessorTypeFromJSON(int componentType);
    static uint accessorDataSizeFromJson(const QString &type);

    static QRenderState *buildStateEnable(int state);
    static QRenderState *buildState(const QString& functionName, const QJsonValue &value, int &type);

    QJsonDocument m_json;
    QString m_basePath;
    bool m_parseDone;
    QString m_defaultScene;

    // multi-hash because our QMeshData corresponds to a single primitive
    // in glTf.
    QMultiHash<QString, QGeometryRenderer*> m_meshDict;

    // GLTF assigns materials at the mesh level, but we do them as siblings,
    // so record the association here for when we instantiate meshes
    QMap<QGeometryRenderer*, QString> m_meshMaterialDict;

    QMap<QString, AccessorData> m_accessorDict;

    QMap<QString, QMaterial*> m_materialCache;

    QMap<QString, BufferData> m_bufferDatas;
    QMap<QString, QBuffer*> m_buffers;

    QMap<QString, QString> m_shaderPaths;
    QMap<QString, QShaderProgram*> m_programs;

    QMap<QString, QTechnique *> m_techniques;
    QMap<QParameter*, ParameterData> m_parameterDataDict;

    QMap<QString, QAbstractTextureProvider*> m_textures;
    QMap<QString, QString> m_imagePaths;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // GLTFPARSER_H
