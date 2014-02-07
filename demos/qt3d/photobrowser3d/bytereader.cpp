/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "bytereader.h"
#include "thumbnailableimage.h"

#include <QByteArray>
#include <QThread>
#include <QImage>
#include <QImageReader>
#include <QUrl>
#include <QFile>
#include <QCoreApplication>
#include <QPainter>

ByteReader::ByteReader()
    : m_stop(0), m_loading(0)
{ }

void ByteReader::loadFile(const ThumbnailableImage &image)
{
    if (!m_stop.load())
    {
        m_loading.ref();

        // FIXME: actually handle remote files
        QUrl url = image.url();
        QString fn = url.toLocalFile();
        int pos = fn.lastIndexOf('.');
        QString ext;
        if (pos != -1)
            ext = fn.mid(pos).toUpper();
        if (ext.isEmpty() ||
                !QImageReader::supportedImageFormats().contains(ext.toLocal8Bit()))
            ext = QString();
        QImage im;
        QFile f(fn);
        QString errorMessage;
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray bytes;
            while (!f.atEnd() & !m_stop.load())
            {
                bytes.append(f.read(1024));
                QCoreApplication::processEvents();
            }
            im = ext.isEmpty() ? QImage::fromData(bytes)
                               : QImage::fromData(bytes, qPrintable(ext));
        }
        else
        {
            errorMessage = tr("Could not read: %1").arg(url.toString());
        }

        QCoreApplication::processEvents();
        if (!m_stop.load())
        {
            if (im.isNull())
            {
                im = QImage(QSize(128, 128), QImage::Format_ARGB32);
                im.fill(qRgba(0, 30, 50, 64));
                QPainter ptr;
                ptr.begin(&im);
                ptr.setBackgroundMode(Qt::TransparentMode);
                if (errorMessage.isEmpty())
                    errorMessage = tr("Could not load: %1").arg(url.toString());
                ptr.setPen(QColor("orange"));
                ptr.drawText(im.rect(), Qt::AlignCenter, errorMessage);
                ptr.end();
            }
            else
            {
                // TODO: Fix size limit
                // This arbitrary size limit is really a debugging/development thing
                // In a real program once you had loaded the full image, the photo-
                // viewer would allow zooming and panning all around in the image
                // so loading a 2896 pixel × 1944 pixel photo would make sense even
                // on a small screen.  For now work with fairly cruddy image quality.
                // Probably the fix is to have a separate load call that re-fetches
                // the full image on a zoom
                QSize workSize(1024, 768);
                Qt::TransformationMode mode = Qt::SmoothTransformation;
                if (QThread::idealThreadCount() < 2)
                {
                    workSize = workSize / 2;
                    mode = Qt::FastTransformation;
                }
                if (im.size().width() > workSize.width() || im.size().height() > workSize.height())
                    im = im.scaled(workSize, Qt::KeepAspectRatio, mode);
            }

            Q_ASSERT(!im.isNull());
            ThumbnailableImage result(image);
            result.setData(im);

            // it would be nice to incur the cost of setThumbnailed() on the image
            // at this point - in the background thread.  Trouble is the atlas is
            // constantly being accessed by the draw loop and to do anything about
            // that would mean locking the GUI thread...

            Q_ASSERT(!result.isNull());

            emit imageLoaded(result);
        }

        m_loading.deref();
    }

    if (m_stop.load())
        emit stopped();

}

void ByteReader::stop()
{
    m_stop.ref();
    if (!m_loading.load())
        emit stopped();
}
