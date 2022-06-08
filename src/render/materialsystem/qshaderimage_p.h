// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERIMAGE_P_H
#define QT3DRENDER_QSHADERIMAGE_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qshaderimage.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;

class QShaderImagePrivate : public Qt3DCore::QNodePrivate
{
public:
    QShaderImagePrivate();
    ~QShaderImagePrivate();

    Q_DECLARE_PUBLIC(QShaderImage)

    Qt3DRender::QAbstractTexture *m_texture;
    int m_mipLevel;
    int m_layer;
    QShaderImage::Access m_access;
    QShaderImage::ImageFormat m_format;
    bool m_layered;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERIMAGE_P_H
