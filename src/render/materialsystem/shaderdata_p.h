/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
