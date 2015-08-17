/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtextureimage.h"
#include "qabstracttextureimage_p.h"
#include <Qt3DCore/private/qurlhelper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTextureImagePrivate: public QAbstractTextureImagePrivate
{
public:
    QTextureImagePrivate()
        : QAbstractTextureImagePrivate()
    {
    }

    Q_DECLARE_PUBLIC(QTextureImage)
    QUrl m_source;
};

class QImageTextureDataFunctor : public QTextureDataFunctor
{
public:
    QImageTextureDataFunctor(const QUrl &url)
        : QTextureDataFunctor()
        , m_url(url)
    {}

    // Will be executed from within a QAspectJob
    TexImageDataPtr operator ()() Q_DECL_FINAL
    {
        TexImageDataPtr dataPtr;
        if (m_url.isLocalFile() || m_url.scheme() == QStringLiteral("qrc")) {
            QString source = QUrlHelper::urlToLocalFileOrQrc(m_url);
            QImage img;
            if (img.load(source)) {
                dataPtr.reset(new TexImageData());
                dataPtr->setImage(img);
            } else {
                qWarning() << "Failed to load image : " << source;
            }
        } else {
            qWarning() << "implement loading from remote URLs";
        }
        return dataPtr;
    }

    bool operator ==(const QTextureDataFunctor &other) const Q_DECL_FINAL
    {
        const QImageTextureDataFunctor *otherFunctor = functor_cast<QImageTextureDataFunctor>(&other);
        return (otherFunctor != Q_NULLPTR && otherFunctor->m_url == m_url);
    }

    QT3D_FUNCTOR(QImageTextureDataFunctor)

private:
    QUrl m_url;
};

/*!
    \class QTextureImage
    \inmodule Qt3DRenderer
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture
    image from an image source.

    It contains the necessary information mipmap level, layer, cube face and
    source URL to load at the proper place data into an OpenGL texture.
 */

/*!
    \qmltype TextureImage
    \instantiates Qt3D::QTextureImage
    \inherits AbstractTextureImage
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture
    image from an image source.
*/

/*!
    Constructs a new Qt3D::QTextureImage instance with \a parent as parent.
 */
QTextureImage::QTextureImage(QNode *parent)
    : QAbstractTextureImage(*new QTextureImagePrivate, parent)
{
}

/*!
  The destructor.
 */
QTextureImage::~QTextureImage()
{
    QNode::cleanup();
}

/*!
    Returns the source url from which data for the texture image will be loaded.
 */
QUrl QTextureImage::source() const
{
    Q_D(const QTextureImage);
    return d->m_source;
}

/*!
  \property Qt3D::QTextureImage::source

  This property holdsthe source url from which data for the texture
  image will be loaded.
*/

/*!
  \qmlproperty url Qt3D.Renderer::TextureImage::source

  This property holdsthe source url from which data for the texture
  image will be loaded.
*/

/*!
    Sets the source url of the texture image to \a source.
    \note This triggers a call to update()
 */
void QTextureImage::setSource(const QUrl &source)
{
    Q_D(QTextureImage);
    if (source != d->m_source) {
        d->m_source = source;
        emit sourceChanged();
        update();
    }
}

/*!
    Returns the Qt3D::QTextureDataFunctorPtr functor to be used by the
    backend to load the texture image data into an OpenGL texture object.
 */
QTextureDataFunctorPtr QTextureImage::dataFunctor() const
{
    return QTextureDataFunctorPtr(new QImageTextureDataFunctor(source()));
}

/*!
  Copies \ref into this texture image.
 */
void QTextureImage::copy(const QNode *ref)
{
    QAbstractTextureImage::copy(ref);
    const QTextureImage *img = static_cast<const QTextureImage *>(ref);
    d_func()->m_source = img->source();
}

} // Qt3D

QT_END_NAMESPACE

