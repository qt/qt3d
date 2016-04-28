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

#include "qtextureimage.h"
#include "qabstracttextureimage.h"
#include "qtextureimage_p.h"
#include "qtexturedata.h"
#include "qtexture.h"
#include "qtexture_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QTextureLoaderPrivate::QTextureLoaderPrivate()
    : QAbstractTexturePrivate()
{
}

/*!
    \class Qt3DRender::QTexture1D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target1D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture1D instance with \a parent as parent.
 */
QTexture1D::QTexture1D(QNode *parent)
    : QAbstractTexture(Target1D, parent)
{
}

/*! \internal */
QTexture1D::~QTexture1D()
{
}

/*!
    \class Qt3DRender::QTexture1DArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target1DArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture1DArray instance with \a parent as parent.
 */
QTexture1DArray::QTexture1DArray(QNode *parent)
    : QAbstractTexture(Target1DArray, parent)
{
}

/*! \internal */
QTexture1DArray::~QTexture1DArray()
{
}

/*!
    \class Qt3DRender::QTexture2D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target2D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2D instance with \a parent as parent.
 */
QTexture2D::QTexture2D(QNode *parent)
    : QAbstractTexture(Target2D, parent)
{
}

/*! \internal */
QTexture2D::~QTexture2D()
{
}

/*!
    \class Qt3DRender::QTexture2DArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target2DArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DArray instance with \a parent as parent.
 */
QTexture2DArray::QTexture2DArray(QNode *parent)
    : QAbstractTexture(Target2DArray, parent)
{
}

/*! \internal */
QTexture2DArray::~QTexture2DArray()
{
}

/*!
    \class Qt3DRender::QTexture3D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target3D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture3D instance with \a parent as parent.
 */
QTexture3D::QTexture3D(QNode *parent)
    : QAbstractTexture(Target3D, parent)
{
}

/*! \internal */
QTexture3D::~QTexture3D()
{
}

/*!
    \class Qt3DRender::QTextureCubeMap
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a TargetCubeMap target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureCubeMap instance with \a parent as parent.
 */
QTextureCubeMap::QTextureCubeMap(QNode *parent)
    : QAbstractTexture(TargetCubeMap, parent)
{
}

/*! \internal */
QTextureCubeMap::~QTextureCubeMap()
{
}

/*!
    \class Qt3DRender::QTextureCubeMapArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a TargetCubeMapArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureCubeMapArray instance with \a parent as parent.
 */
QTextureCubeMapArray::QTextureCubeMapArray(QNode *parent)
    : QAbstractTexture(TargetCubeMapArray, parent)
{
}

/*! \internal */
QTextureCubeMapArray::~QTextureCubeMapArray()
{
}

/*!
    \class Qt3DRender::QTexture2DMultisample
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target2DMultisample target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DMultisample instance with \a parent as parent.
 */
QTexture2DMultisample::QTexture2DMultisample(QNode *parent)
    : QAbstractTexture(Target2DMultisample, parent)
{
}

/*! \internal */
QTexture2DMultisample::~QTexture2DMultisample()
{
}

/*!
    \class Qt3DRender::QTexture2DMultisampleArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a Target2DMultisampleArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DMultisampleArray instance with \a parent as parent.
 */
QTexture2DMultisampleArray::QTexture2DMultisampleArray(QNode *parent)
    : QAbstractTexture(Target2DMultisampleArray, parent)
{
}

/*! \internal */
QTexture2DMultisampleArray::~QTexture2DMultisampleArray()
{
}

/*!
    \class Qt3DRender::QTextureRectangle
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a TargetRectangle target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureRectangle instance with \a parent as parent.
 */
QTextureRectangle::QTextureRectangle(QNode *parent)
    : QAbstractTexture(TargetRectangle, parent)
{
}

/*! \internal */
QTextureRectangle::~QTextureRectangle()
{
}

/*!
    \class Qt3DRender::QTextureBuffer
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTexture with a TargetBuffer target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureBuffer instance with \a parent as parent.
 */
QTextureBuffer::QTextureBuffer(QNode *parent)
    : QAbstractTexture(TargetBuffer, parent)
{
}

/*! \internal */
QTextureBuffer::~QTextureBuffer()
{
}

QTextureLoader::QTextureLoader(QNode *parent)
    : QAbstractTexture(*new QTextureLoaderPrivate, parent)
{
    d_func()->m_target = TargetAutomatic;
}

/*! \internal */
QTextureLoader::~QTextureLoader()
{
}

QUrl QTextureLoader::source() const
{
    Q_D(const QTextureLoader);
    return d->m_source;
}

void QTextureLoader::setSource(const QUrl& source)
{
    Q_D(QTextureLoader);
    if (source != d->m_source) {
        d->m_source = source;
        d->m_dataFunctor.reset(new QTextureFromSourceGenerator(source));
        emit sourceChanged(source);
    }
}

bool QTextureFromSourceGenerator::operator ==(const QTextureGenerator &other) const
{
    const QTextureFromSourceGenerator *otherFunctor = functor_cast<QTextureFromSourceGenerator>(&other);
    return (otherFunctor != Q_NULLPTR && otherFunctor->m_url == m_url);

}


QTextureFromSourceGenerator::QTextureFromSourceGenerator(const QUrl &url)
    : QTextureGenerator()
    , m_url(url)
{
}

// Note: Maybe this should return a struct containing information such as
// the format, number of layers ....
// This would also give more flexibility for the future
QTextureDataPtr QTextureFromSourceGenerator::operator ()()
{
    QTextureDataPtr generatedData = QTextureDataPtr::create();
    // TO DO: Make it handle more formats and formats with nested images

    m_status = QAbstractTexture::Loading;

    if (m_url.isLocalFile() || m_url.scheme() == QStringLiteral("qrc")) {
        QString source = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(m_url);
        QVector<QTextureImageDataPtr> images;
        QTextureImageDataPtr dataPtr = QTextureImageDataPtr::create();
        // TO DO: Ideally a setter shouldn't return a value
        // Separate that into isCompressedFile and setCompressedFile
        // Set compressed fill the QTextImageData if possible,
        // returns false if it failed or is not a compressed format

        // TO DO: Make separateSetCompressed from the loading of a compressed file
        // Make such loader set the property formats, targets and sizes in the generatedData

        if (!dataPtr->setCompressedFile(source)) {
            // Try to load using QImage
            QImage img;
            if (img.load(source))
                dataPtr->setImage(img);
            generatedData->setTarget(QAbstractTexture::Target2D);
            generatedData->setWidth(img.width());
            generatedData->setHeight(img.height());
            generatedData->setDepth(1);
        }

        if (dataPtr->data().length() > 0) {
            m_status = QAbstractTexture::Ready;
            generatedData->addImageData(dataPtr);
            images.push_back(dataPtr);
        } else {
            m_status = QAbstractTexture::Error;
            qWarning() << "Failed to load image : " << source;
        }
    } else {
        m_status = QAbstractTexture::Error;
        qWarning() << "implement loading from remote URLs";
    }
    return generatedData;
}

} // namespace Qt3DRender

QT_END_NAMESPACE


