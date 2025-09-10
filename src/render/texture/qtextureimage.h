// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTUREIMAGE_H
#define QT3DRENDER_QTEXTUREIMAGE_H

#include <QtCore/QUrl>
#include <Qt3DRender/qabstracttextureimage.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImagePrivate;

class Q_3DRENDERSHARED_EXPORT QTextureImage : public QAbstractTextureImage
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool mirrored READ isMirrored WRITE setMirrored NOTIFY mirroredChanged)

public:
    explicit QTextureImage(Qt3DCore::QNode *parent = nullptr);
    ~QTextureImage();

    enum Status {
        None = 0,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    QUrl source() const;
    Status status() const;
    bool isMirrored() const;

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setMirrored(bool mirrored);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(Status status);
    void mirroredChanged(bool mirrored);

protected:
    // TO DO Qt6 -> should be on private class
    void setStatus(Status status);

private:
    QTextureImageDataGeneratorPtr dataGenerator() const override;
    Q_DECLARE_PRIVATE(QTextureImage)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTUREIMAGE_H
