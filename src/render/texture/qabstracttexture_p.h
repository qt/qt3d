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
****************************************************************************/

#ifndef QT3DRENDER_QABSTRACTTEXTURE_P_H
#define QT3DRENDER_QABSTRACTTEXTURE_P_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtexturewrapmode.h>
#include <Qt3DRender/qtexturegenerator.h>
#include <Qt3DRender/qtexturedataupdate.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QAbstractTexturePrivate : public Qt3DCore::QNodePrivate
{
public :
    QAbstractTexturePrivate();

    Q_DECLARE_PUBLIC(QAbstractTexture)

    QAbstractTexture::Target m_target;
    QAbstractTexture::TextureFormat m_format;
    int m_width, m_height, m_depth;
    bool m_autoMipMap;

    QVector<QTextureImageDataPtr> m_data;

    QAbstractTexture::Filter m_minFilter, m_magFilter;
    // FIXME, store per direction
    QTextureWrapMode m_wrapMode;
    QAbstractTexture::Status m_status;
    float m_maximumAnisotropy;
    QAbstractTexture::ComparisonFunction m_comparisonFunction;
    QAbstractTexture::ComparisonMode m_comparisonMode;
    QVector<QAbstractTextureImage *> m_textureImages;
    int m_layers;
    int m_samples;
    int m_mipmapLevels;

    int m_sharedTextureId;

    QAbstractTexture::HandleType m_handleType;
    QVariant m_handle;

    QTextureGeneratorPtr dataFunctor() const;
    void setDataFunctor(const QTextureGeneratorPtr &generator);

    void setStatus(QAbstractTexture::Status status);
    void setHandle(const QVariant &handle);
    void setHandleType(QAbstractTexture::HandleType type);

    QVector<QTextureDataUpdate> m_pendingDataUpdates;

private:
    QTextureGeneratorPtr m_dataFunctor;
};

struct QAbstractTextureData
{
    QAbstractTexture::Target target;
    QAbstractTexture::TextureFormat format;
    int width;
    int height;
    int depth;
    bool autoMipMap;
    QAbstractTexture::Filter minFilter;
    QAbstractTexture::Filter magFilter;
    QTextureWrapMode::WrapMode wrapModeX;
    QTextureWrapMode::WrapMode wrapModeY;
    QTextureWrapMode::WrapMode wrapModeZ;
    float maximumAnisotropy;
    QAbstractTexture::ComparisonFunction comparisonFunction;
    QAbstractTexture::ComparisonMode comparisonMode;
    Qt3DCore::QNodeIdVector textureImageIds;
    int layers;
    int samples;
    int sharedTextureId;
    QTextureGeneratorPtr dataFunctor;
    QVector<QTextureDataUpdate> initialDataUpdates;
};

} // QT3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTTEXTURE_P_H

