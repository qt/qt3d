/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "imageloader.h"
#include "imagemanager.h"
#include "bytereader.h"

#include <QFileInfo>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QImageReader>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

ImageLoader::ImageLoader()
{
    m_stop = 0;
}

ImageLoader::~ImageLoader()
{
    // nothing to do here
}

ThumbnailableImage ImageLoader::image() const
{
    return m_image;
}

void ImageLoader::setImage(const ThumbnailableImage &image)
{
    m_image = image;
    if (!m_stop && isRunning())
        emit readRequired(image);
}

void ImageLoader::stop()
{
    m_stop.ref();
    emit stopLoading();
}

void ImageLoader::queueInitialImage()
{
    emit readRequired(m_image);
}

void ImageLoader::unusedTimeout()
{
    emit unused();
}

void ImageLoader::run()
{
    ByteReader reader;
    connect(this, SIGNAL(readRequired(ThumbnailableImage)),
            &reader, SLOT(loadFile(ThumbnailableImage)));
    connect(&reader, SIGNAL(imageLoaded(ThumbnailableImage)),
            this, SIGNAL(imageLoaded(ThumbnailableImage)));

    connect(this, SIGNAL(stopLoading()), &reader, SLOT(stop()));
    connect(&reader, SIGNAL(stopped()), this, SLOT(quit()));

    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(unusedTimeout()));
    timer.start(2 * 60 * 1000);

    if (!m_image.isNull())
        QTimer::singleShot(0, this, SLOT(queueInitialImage()));

    exec();
}
