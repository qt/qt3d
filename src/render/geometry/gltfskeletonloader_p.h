// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_GLTFSKELETONLOADER_P_H
#define QT3DRENDER_RENDER_GLTFSKELETONLOADER_P_H

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

#include <QtGlobal>
#include <Qt3DCore/qattribute.h>

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qjsondocument.h>

#include <Qt3DRender/private/skeletondata_p.h>
#include <Qt3DCore/private/sqt_p.h>

QT_BEGIN_NAMESPACE

class QJsonObject;

namespace Qt3DRender {
namespace Render {

class GLTFSkeletonLoader
{
    class BufferData
    {
    public:
        BufferData();
        explicit BufferData(const QJsonObject &json);

        quint64 byteLength;
        QString path;
        QByteArray data;
    };

    class BufferView
    {
    public:
        BufferView();
        explicit BufferView(const QJsonObject &json);

        int bufferIndex;
        quint64 byteOffset;
        quint64 byteLength;
        int target; // Only for per vertex attributes
    };

    class AccessorData
    {
    public:
        AccessorData();
        explicit AccessorData(const QJsonObject &json);

        int bufferViewIndex;
        Qt3DCore::QAttribute::VertexBaseType type;
        uint dataSize;
        int count;
        int byteOffset;
        int byteStride; // Only for per vertex attributes

        // TODO: Extend to support sparse accessors
    };

    class Skin
    {
    public:
        Skin();
        explicit Skin(const QJsonObject &json);

        QString name;
        int inverseBindAccessorIndex;
        std::vector<int> jointNodeIndices;
    };

    class Node
    {
    public:
        Node();
        explicit Node(const QJsonObject &json);

        Qt3DCore::Sqt localTransform;
        std::vector<int> childNodeIndices;
        QString name;
        int parentNodeIndex;
        int cameraIndex;
        int meshIndex;
        int skinIndex;
    };

public:
    GLTFSkeletonLoader();

    bool load(QIODevice *ioDev);

    SkeletonData createSkeleton(const QString &skeletonName);

private:
    static Qt3DCore::QAttribute::VertexBaseType accessorTypeFromJSON(int componentType);
    static uint accessorTypeSize(Qt3DCore::QAttribute::VertexBaseType componentType);
    static uint accessorDataSizeFromJson(const QString &type);

    struct RawData
    {
        const char *data;
        quint64 byteLength;
    };

    void setBasePath(const QString &path);
    bool setJSON(const QJsonDocument &json);

    bool parse();
    bool parseGLTF2();
    void cleanup();

    bool processJSONBuffer(const QJsonObject &json);
    bool processJSONBufferView(const QJsonObject &json);
    bool processJSONAccessor(const QJsonObject &json);
    bool processJSONSkin(const QJsonObject &json);
    bool processJSONNode(const QJsonObject &json);
    void setupNodeParentLinks();
    QByteArray resolveLocalData(const QString &path) const;

    SkeletonData createSkeletonFromSkin(const Skin &skin) const;
    QMatrix4x4 inverseBindMatrix(const Skin &skin, int jointIndex) const;
    RawData accessorData(int accessorIndex, int index) const;

    QJsonDocument m_json;
    QString m_basePath;
    std::vector<BufferData> m_bufferDatas;
    std::vector<BufferView> m_bufferViews;
    std::vector<AccessorData> m_accessors;
    std::vector<Skin> m_skins;
    std::vector<Node> m_nodes;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GLTFSKELETONLOADER_P_H
