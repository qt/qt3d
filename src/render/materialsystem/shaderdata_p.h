// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_SHADERDATA_P_H
#define QT3DRENDER_RENDER_SHADERDATA_P_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DCore/private/matrix4x4_p.h>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class GraphicsContext;
class GLBuffer;
class NodeManagers;

class Q_3DRENDERSHARED_PRIVATE_EXPORT ShaderData : public BackendNode
{
public:
    enum TransformType {
        NoTransform = -1,
        ModelToEye = 0,
        ModelToWorld,
        ModelToWorldDirection
    };
    struct PropertyValue {
        QVariant value;
        bool isNode;
        bool isArray;
        bool isTransformed;
        QString transformedPropertyName;
    };

    ShaderData();
    ~ShaderData();

    const QHash<QString, PropertyValue> &properties() const { return m_originalProperties; }

    // Called by FramePreparationJob
    void updateWorldTransform(const Matrix4x4 &worldMatrix);

    QVariant getTransformedProperty(const PropertyValue *v, const Matrix4x4 &viewMatrix) const noexcept;

    // Unit tests purposes only
    TransformType propertyTransformType(const QString &name) const;

    void setManagers(NodeManagers *managers);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

#ifdef Q_OS_WIN
    // To get MSVC to compile even though we don't need any cleanup
    void cleanup() {}
#endif

    // Block.Property nameId, property nameId, PropertyValue *
    using PropertyValuesForBlock = std::vector<std::tuple<int, int, const PropertyValue *>>;

    bool hasPropertyValuesForBlock(int blockNameId) const;
    const PropertyValuesForBlock &propertyValuesForBlock(int blockNameId) const;
    void generatePropertyValuesForBlock(const QString &blockName);

protected:
    PropertyReaderInterfacePtr m_propertyReader;

    // 1 to 1 match with frontend properties
    QHash<QString, PropertyValue> m_originalProperties;

    // BlockNameId to array of pair of BlockName+PropertyName PropertyValue
    std::unordered_map<int, PropertyValuesForBlock> m_blockNameToPropertyValues;

    Matrix4x4 m_worldMatrix;
    NodeManagers *m_managers;

    static ShaderData *lookupResource(NodeManagers *managers, Qt3DCore::QNodeId id);
    ShaderData *lookupResource(Qt3DCore::QNodeId id);

    friend class RenderShaderDataFunctor;

private:
    mutable std::shared_mutex m_lock;
};

class RenderShaderDataFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderShaderDataFunctor(AbstractRenderer *renderer, NodeManagers *managers);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    NodeManagers *m_managers;
    AbstractRenderer *m_renderer;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::ShaderData*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_RENDER_SHADERDATA_P_H
