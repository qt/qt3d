// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <Qt3DRender/qtexturedataupdate.h>
#include <Qt3DRender/private/qtexturegenerator_p.h>
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

    QList<QTextureImageDataPtr> m_data;

    QAbstractTexture::Filter m_minFilter, m_magFilter;
    // FIXME, store per direction
    QTextureWrapMode m_wrapMode;
    QAbstractTexture::Status m_status;
    float m_maximumAnisotropy;
    QAbstractTexture::ComparisonFunction m_comparisonFunction;
    QAbstractTexture::ComparisonMode m_comparisonMode;
    QList<QAbstractTextureImage *> m_textureImages;
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

    QList<QTextureDataUpdate> m_pendingDataUpdates;

private:
    QTextureGeneratorPtr m_dataFunctor;
};

} // QT3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTTEXTURE_P_H

