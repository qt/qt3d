// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERATTRIBUTE_H
#define QT3DRENDER_RENDER_RENDERATTRIBUTE_H

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
#include <Qt3DCore/qattribute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT Attribute : public BackendNode
{
public:
    Attribute();
    ~Attribute();

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline Qt3DCore::QNodeId bufferId() const { return m_bufferId; }
    inline QString name() const { return m_name; }
    inline int nameId() const { return m_nameId; }
    inline Qt3DCore::QAttribute::VertexBaseType vertexBaseType() const { return m_vertexBaseType; }
    inline uint vertexSize() const { return m_vertexSize; }
    inline uint count() const { return m_count; }
    inline uint byteStride() const { return m_byteStride; }
    inline uint byteOffset() const { return m_byteOffset; }
    inline uint divisor() const { return m_divisor; }
    inline Qt3DCore::QAttribute::AttributeType attributeType() const { return m_attributeType; }
    inline bool isDirty() const { return m_attributeDirty; }
    void unsetDirty();

private:
    Qt3DCore::QNodeId m_bufferId;
    QString m_name;
    int m_nameId;
    Qt3DCore::QAttribute::VertexBaseType m_vertexBaseType;
    uint m_vertexSize;
    uint m_count;
    uint m_byteStride;
    uint m_byteOffset;
    uint m_divisor;
    Qt3DCore::QAttribute::AttributeType m_attributeType;
    bool m_attributeDirty;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERATTRIBUTE_H
