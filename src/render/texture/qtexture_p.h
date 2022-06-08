// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTURE_P_H
#define QT3DRENDER_QTEXTURE_P_H

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

#include <Qt3DCore/QNodeId>
#include <Qt3DCore/private/qdownloadhelperservice_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/qtexturegenerator_p.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QTextureLoaderPrivate : public QAbstractTexturePrivate
{
public:
    QTextureLoaderPrivate();

    Q_DECLARE_PUBLIC(QTextureLoader)

    void setScene(Qt3DCore::QScene *scene) override;
    void updateGenerator();

    QUrl m_source;
    bool m_mirrored;
};

class QTextureFromSourceGenerator;
typedef QSharedPointer<QTextureFromSourceGenerator> QTextureFromSourceGeneratorPtr;

class Q_AUTOTEST_EXPORT TextureDownloadRequest : public Qt3DCore::QDownloadRequest
{
public:
    TextureDownloadRequest(const QTextureFromSourceGeneratorPtr &functor,
                           const QUrl &url,
                           Qt3DCore::QAspectEngine *engine,
                           Qt3DCore::QNodeId texNodeId);

    void onCompleted() override;

private:
    QTextureFromSourceGeneratorPtr m_functor;
    Qt3DCore::QAspectEngine *m_engine;
    Qt3DCore::QNodeId m_texNodeId;
};

class Q_AUTOTEST_EXPORT QTextureFromSourceGenerator : public QTextureGenerator,
                                                      public QEnableSharedFromThis<QTextureFromSourceGenerator>
{
public:
    explicit QTextureFromSourceGenerator(QTextureLoader *textureLoader,
                                         Qt3DCore::QAspectEngine *engine,
                                         Qt3DCore::QNodeId textureId);

    QTextureFromSourceGenerator(const QTextureFromSourceGenerator &other);

    QTextureDataPtr operator ()() override;
    bool operator ==(const QTextureGenerator &other) const override;
    inline QAbstractTexture::Status status() const { return m_status; }

    QT3D_FUNCTOR(QTextureFromSourceGenerator)

    QUrl url() const;
    bool isMirrored() const;

private:
    friend class TextureDownloadRequest;

    QUrl m_url;
    QAbstractTexture::Status m_status;
    bool m_mirrored;

    QByteArray m_sourceData;
    Qt3DCore::QNodeId m_texture;
    Qt3DCore::QAspectEngine *m_engine;

    // Options that can be overridden on TextureLoader when loading
    QAbstractTexture::TextureFormat m_format;
};

class Q_AUTOTEST_EXPORT TextureLoadingHelper
{
public:
    static QTextureImageDataPtr loadTextureData(const QUrl &source, bool allow3D, bool mirrored);
    static QTextureImageDataPtr loadTextureData(QIODevice *data, const QString& suffix,
                                                bool allow3D, bool mirrored);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTURE_P_H
