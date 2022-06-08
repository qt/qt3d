// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_SHADERIMAGE_P_H
#define QT3DRENDER_RENDER_SHADERIMAGE_P_H

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
#include <Qt3DRender/qshaderimage.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_AUTOTEST_EXPORT ShaderImage : public BackendNode
{
public:
    ShaderImage();

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeId textureId() const { return m_textureId; }
    int mipLevel() const { return m_mipLevel; }
    int layer() const { return m_layer; }
    bool layered() const { return m_layered; }
    QShaderImage::Access access() const { return m_access; }
    QShaderImage::ImageFormat format() const { return m_format; }

    // For Unit Test purposes only
#ifdef QT_BUILD_INTERNAL
    void setTextureId(Qt3DCore::QNodeId id) { m_textureId = id; }
    void setMipLevel(int level) { m_mipLevel = level; }
    void setLayer(int layer) { m_layer = layer; }
    void setLayered(bool layered) { m_layered = layered; }
    void setAccess(QShaderImage::Access access) { m_access = access; }
    void setFormat(QShaderImage::ImageFormat format) { m_format = format; }
#endif

private:
    Qt3DCore::QNodeId m_textureId;
    int m_mipLevel;
    int m_layer;
    bool m_layered;
    QShaderImage::Access m_access;
    QShaderImage::ImageFormat m_format;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SHADERIMAGE_P_H
