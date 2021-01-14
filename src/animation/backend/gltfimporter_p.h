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

#ifndef QT3DANIMATION_ANIMATION_GLTFIMPORTER_H
#define QT3DANIMATION_ANIMATION_GLTFIMPORTER_H

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
#include <Qt3DAnimation/private/fcurve_p.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/private/sqt_p.h>
#include <Qt3DCore/private/qmath3d_p.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>

QT_BEGIN_NAMESPACE

class QIODevice;

namespace Qt3DAnimation {
namespace Animation {

class GLTFImporter
{
public:
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

        quint64 byteOffset;
        quint64 byteLength;
        int bufferIndex;
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
        QList<int> jointNodeIndices;
    };

    class Channel
    {
    public:
        Channel();
        explicit Channel(const QJsonObject &json);

        int samplerIndex;
        int targetNodeIndex;
        QString targetProperty;
    };

    class Sampler
    {
    public:
        Sampler();
        explicit Sampler(const QJsonObject &json);

        enum InterpolationMode {
            Linear,
            Step,
            CatmullRomSpline,
            CubicSpline
        };

        QString interpolationModeString() const;

        int inputAccessorIndex;
        int outputAccessorIndex;
        InterpolationMode interpolationMode;
    };

    class Animation
    {
    public:
        Animation();
        explicit Animation(const QJsonObject &json);

        QString name;
        QList<Channel> channels;
        QList<Sampler> samplers;
    };

    class Node
    {
    public:
        Node();
        explicit Node(const QJsonObject &json);

        Qt3DCore::Sqt localTransform;
        QList<int> childNodeIndices;
        QString name;
        int parentNodeIndex;
        int cameraIndex;
        int meshIndex;
        int skinIndex;
    };

    GLTFImporter();

    bool load(QIODevice *ioDev);
    const QList<Animation> animations() const { return m_animations; }

    struct AnimationNameAndChannels
    {
        QString name;
        QVector<Qt3DAnimation::Animation::Channel> channels;
    };
    AnimationNameAndChannels createAnimationData(int animationIndex, const QString &animationName = QString()) const;

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
    QHash<int, int> createNodeIndexToJointIndexMap(const Skin &skin) const;

    bool processJSONBuffer(const QJsonObject &json);
    bool processJSONBufferView(const QJsonObject &json);
    bool processJSONAccessor(const QJsonObject &json);
    bool processJSONSkin(const QJsonObject &json);
    bool processJSONAnimation(const QJsonObject &json);
    bool processJSONNode(const QJsonObject &json);
    void setupNodeParentLinks();
    QByteArray resolveLocalData(const QString &path) const;

    RawData accessorData(int accessorIndex, int index) const;

    QJsonDocument m_json;
    QString m_basePath;
    QList<BufferData> m_bufferDatas;
    QList<BufferView> m_bufferViews;
    QList<AccessorData> m_accessors;
    QList<Skin> m_skins;
    QList<Animation> m_animations;
    QList<Node> m_nodes;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_GLTFIMPORTER_H
