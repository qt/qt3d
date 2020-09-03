/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H
#define QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <qbytearray.h>

QT_BEGIN_NAMESPACE
class QRhiBuffer;
namespace Qt3DRender {

namespace Render {

namespace Rhi {

class SubmissionContext;

class RHIBuffer
{
public:
    RHIBuffer();

    enum Type {
        ArrayBuffer         = 1 << 0,
        UniformBuffer       = 1 << 1,
        IndexBuffer         = 1 << 2,
        ShaderStorageBuffer = 1 << 3,
        PixelPackBuffer     = 1 << 4,
        PixelUnpackBuffer   = 1 << 5,
        DrawIndirectBuffer  = 1 << 6
    };

    bool bind(SubmissionContext *ctx, Type t);
    void destroy();
    void destroyOrphaned();
    void allocate(const QByteArray &data, bool dynamic = true);
    void update(const QByteArray &data, int offset = 0);
    QByteArray download(SubmissionContext *ctx, uint size);
    void cleanup();

    qsizetype size() const { return m_allocSize; }
    QRhiBuffer *rhiBuffer() const noexcept { return m_rhiBuffer; }

private:
    void orphan();

    uint m_bufferId;
    bool m_dynamic;
    qsizetype m_allocSize {};

    QRhiBuffer *m_rhiBuffer {};

    std::vector<QRhiBuffer *> m_buffersToCleanup;

    std::vector<std::pair<QByteArray /*data*/, int /*offset*/>> m_datasToUpload;
};

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H
