// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTUREIMAGE_P_H
#define QT3DRENDER_QTEXTUREIMAGE_P_H

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

#include <QtCore/QDateTime>

#include <Qt3DRender/private/qabstracttextureimage_p.h>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QTextureImagePrivate : public QAbstractTextureImagePrivate
{
public:
    QTextureImagePrivate()
        : QAbstractTextureImagePrivate()
        , m_status(QTextureImage::None)
        , m_mirrored(true)
    {
    }

    Q_DECLARE_PUBLIC(QTextureImage)

    QUrl m_source;
    QTextureImage::Status m_status;
    bool m_mirrored;
};

class Q_AUTOTEST_EXPORT QImageTextureDataFunctor : public QTextureImageDataGenerator
{
public:
    explicit QImageTextureDataFunctor(const QUrl &url, bool mirrored);
    // Will be executed from within a QAspectJob
    QTextureImageDataPtr operator ()() final;
    bool operator ==(const QTextureImageDataGenerator &other) const final;
    inline QTextureImage::Status status() const { return m_status; }
    QT3D_FUNCTOR(QImageTextureDataFunctor)

    QUrl url() const;
    bool isMirrored() const;

private:
    QUrl m_url;
    QDateTime m_lastModified;
    QTextureImage::Status m_status;
    bool m_mirrored;
};

typedef QSharedPointer<QImageTextureDataFunctor> QImageTextureDataFunctorPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTUREIMAGE_P_H
