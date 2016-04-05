/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DRender/private/qabstracttextureimage_p.h>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/private/qurlhelper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImagePrivate : public QAbstractTextureImagePrivate
{
public:
    QTextureImagePrivate()
        : QAbstractTextureImagePrivate()
        , m_status(QTextureImage::Loading)
    {
    }

    Q_DECLARE_PUBLIC(QTextureImage)

    QUrl m_source;
    QTextureImage::Status m_status;
};

class QImageTextureDataFunctor : public QTextureImageDataGenerator
{
public:
    QImageTextureDataFunctor(const QUrl &url)
        : QTextureImageDataGenerator()
        , m_url(url)
    {}

    // Will be executed from within a QAspectJob
    QTexImageDataPtr operator ()() Q_DECL_FINAL
    {
        QTexImageDataPtr dataPtr;
        if (m_url.isLocalFile() || m_url.scheme() == QStringLiteral("qrc")) {
            QString source = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(m_url);
            dataPtr.reset(new QTexImageData());
            if (dataPtr->setCompressedFile(source))
                return dataPtr;
            QImage img;
            if (img.load(source)) {
                dataPtr->setImage(img);
                return dataPtr;
            }
            dataPtr.reset();
            qWarning() << "Failed to load image : " << source;
        } else {
            qWarning() << "implement loading from remote URLs";
        }
        return dataPtr;
    }

    bool operator ==(const QTextureImageDataGenerator &other) const Q_DECL_FINAL
    {
        const QImageTextureDataFunctor *otherFunctor = functor_cast<QImageTextureDataFunctor>(&other);
        return (otherFunctor != Q_NULLPTR && otherFunctor->m_url == m_url);
    }

    QT3D_FUNCTOR(QImageTextureDataFunctor)

private:
    QUrl m_url;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTUREIMAGE_P_H
