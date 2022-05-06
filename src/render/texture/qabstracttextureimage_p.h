/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QABSTRACTTEXTUREIMAGE_P_H
#define QT3DRENDER_QABSTRACTTEXTUREIMAGE_P_H

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
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTextureImage;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QAbstractTextureImagePrivate : public Qt3DCore::QNodePrivate
{
public:
    QAbstractTextureImagePrivate();
    ~QAbstractTextureImagePrivate();

    Q_DECLARE_PUBLIC(QAbstractTextureImage)

    int m_mipLevel;
    int m_layer;
    QAbstractTexture::CubeMapFace m_face;
    QTextureImageDataGeneratorPtr dataGenerator() const;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTTEXTUREIMAGE_P_H

