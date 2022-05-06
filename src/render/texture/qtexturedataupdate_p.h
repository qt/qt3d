/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
