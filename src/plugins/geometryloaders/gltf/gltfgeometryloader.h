// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef GLTFGEOMETRYLOADER_H
#define GLTFGEOMETRYLOADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QHash>
#include <QtCore/QJsonDocument>

#include <Qt3DRender/private/qgeometryloaderinterface_p.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>

#include <private/qlocale_tools_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QGeometry;
}

namespace Qt3DRender {

#define GLTFGEOMETRYLOADER_EXT QLatin1String("gltf")
#define JSONGEOMETRYLOADER_EXT QLatin1String("json")
#define QGLTFGEOMETRYLOADER_EXT QLatin1String("qgltf")

class QCamera;
class QCameraLens;
class QMaterial;
class QShaderProgram;
class QEffect;
class QAbstractTexture;
class QRenderState;
class QTechnique;
class QParameter;
class QGeometryRenderer;

class GLTFGeometryLoader : public QGeometryLoaderInterface
{
    class BufferData
    {
    public:
        BufferData();
        explicit BufferData(const QJsonObject &json);

        quint64 length;
        QString path;
        QByteArray *data;
        // type if ever useful
    };

    class ParameterData
    {
    public:
        ParameterData();
        explicit ParameterData(const QJsonObject &json);

        QString semantic;
        int type;
    };

    class AccessorData
    {
    public:
        AccessorData();
        explicit AccessorData(const QJsonObject &json);

        QString bufferViewName;
        int bufferViewIndex;
        Qt3DCore::QAttribute::VertexBaseType type;
        uint dataSize;
        int count;
        int offset;
        int stride;
    };

    struct Gltf1
    {
        QHash<QString, AccessorData> m_accessorDict;
        QHash<QString, BufferData> m_bufferDatas;
        QHash<QString, Qt3DCore::QBuffer*> m_buffers;
    };

    struct Gltf2
    {
        QVector<BufferData> m_bufferDatas;
        QVector<Qt3DCore::QBuffer*> m_buffers;
        QVector<AccessorData> m_accessors;
    };

    Q_OBJECT
public:
    GLTFGeometryLoader();
    ~GLTFGeometryLoader();

    Qt3DCore::QGeometry *geometry() const final;

    bool load(QIODevice *ioDev, const QString &subMesh = QString()) final;

protected:
    void setBasePath(const QString &path);
    bool setJSON(const QJsonDocument &json);

    static QString standardAttributeNameFromSemantic(const QString &semantic);

    void parse();
    void parseGLTF1();
    void parseGLTF2();
    void cleanup();

    void processJSONBuffer(const QString &id, const QJsonObject &json);
    void processJSONBufferView(const QString &id, const QJsonObject &json);
    void processJSONAccessor(const QString &id, const QJsonObject &json);
    void processJSONMesh(const QString &id, const QJsonObject &json);

    void loadBufferData();
    void unloadBufferData();

    void processJSONBufferV2(const QJsonObject &json);
    void processJSONBufferViewV2(const QJsonObject &json);
    void processJSONAccessorV2(const QJsonObject &json);
    void processJSONMeshV2(const QJsonObject &json);

    void loadBufferDataV2();
    void unloadBufferDataV2();

    QByteArray resolveLocalData(const QString &path) const;

    static Qt3DCore::QAttribute::VertexBaseType accessorTypeFromJSON(int componentType);
    static uint accessorDataSizeFromJson(const QString &type);

private:
    QJsonDocument m_json;
    QString m_basePath;
    QString m_mesh;

    Gltf1 m_gltf1;
    Gltf2 m_gltf2;

    Qt3DCore::QGeometry *m_geometry;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // GLTFGEOMETRYLOADER_H
