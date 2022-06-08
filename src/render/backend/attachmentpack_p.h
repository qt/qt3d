// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ATTACHMENTPACK_P_H
#define QT3DRENDER_RENDER_ATTACHMENTPACK_P_H

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

#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

struct Q_3DRENDERSHARED_PRIVATE_EXPORT Attachment
{
    Attachment()
        : m_mipLevel(0)
        , m_layer(0)
        , m_point(QRenderTargetOutput::Color0)
        , m_face(QAbstractTexture::CubeMapNegativeX)
    {}

    QString m_name;
    int m_mipLevel;
    int m_layer;
    Qt3DCore::QNodeId m_textureUuid;
    QRenderTargetOutput::AttachmentPoint m_point;
    QAbstractTexture::CubeMapFace m_face;
};

class RenderTarget;
class RenderTargetSelector;
class AttachmentManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT AttachmentPack
{
public:
    AttachmentPack();
    AttachmentPack(const RenderTarget *target,
                   AttachmentManager *attachmentManager,
                   const QList<QRenderTargetOutput::AttachmentPoint> &drawBuffers = {});

    const std::vector<Attachment> &attachments() const { return m_attachments; }
    const std::vector<int> &getGlDrawBuffers() const { return m_drawBuffers; }

    // return index of given attachment within actual draw buffers list
    int getDrawBufferIndex(QRenderTargetOutput::AttachmentPoint attachmentPoint) const;

private:
    std::vector<Attachment> m_attachments;
    std::vector<int> m_drawBuffers;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator ==(const Attachment &a, const Attachment &b);
Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator !=(const Attachment &a, const Attachment &b);

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator ==(const AttachmentPack &packA, const AttachmentPack &packB);
Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator !=(const AttachmentPack &packA, const AttachmentPack &packB);

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ATTACHMENTPACK_P_H
