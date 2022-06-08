// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTUREDATAUPDATE_P_H
#define QT3DRENDER_QTEXTUREDATAUPDATE_P_H

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

#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/qabstracttexture.h>
#include <private/qglobal_p.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureDataUpdatePrivate : public QSharedData
{
public:
    int m_x = 0;
    int m_y = 0;
    int m_z = 0;
    int m_layer = 0;
    int m_mipLevel = 0;
    QAbstractTexture::CubeMapFace m_face = QAbstractTexture::CubeMapPositiveX;
    QTextureImageDataPtr m_data;
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTUREDATAUPDATE_P_H
