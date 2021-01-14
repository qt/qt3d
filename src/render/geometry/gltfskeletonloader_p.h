/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
