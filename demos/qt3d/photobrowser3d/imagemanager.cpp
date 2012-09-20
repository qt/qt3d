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

#include "imagemanager.h"
#include "qatlas.h"
#include "filescanner.h"
#include "threadpool.h"

#include <QTime>
#include <QTimer>
#include <QDir>

ImageManager::ImageManager()
{
}

ImageManager::~ImageManager()
{
}

// INVARIANT: only ever called before the run() function is started
// therefore no need for synchronized url
void ImageManager::setImageBaseUrl(const QUrl &url)
{
    Q_ASSERT(!isRunning());
    m_url = url;
}

/*!
    Stop the running threads if any, then sit waiting in the event loop
    for a quit call.
*/
void ImageManager::stop()
{
    emit stopAll();
}

void ImageManager::scanForFiles()
{
    // TODO: In a real app there would be a way to detect new files arriving
    // and trigger a rescan to pick these new files up.  Here we just scan
    // once and then destroy the scanner, to save on resources.

#ifndef QT_USE_TEST_IMAGES
    // TODO: If the amount of files is large and the app is quit early the
    // scanner could still be going when the threadpool quits.  For now
    // assume its ok...
    FileScanner *scanner = new FileScanner;
    scanner->setBaseUrl(m_url);
    connect(scanner, SIGNAL(imageUrl(QUrl)), this, SIGNAL(imageUrl(QUrl)));
    connect(scanner, SIGNAL(finished()), scanner, SLOT(deleteLater()));
    connect(this, SIGNAL(stopAll()), scanner, SLOT(stop()));
    scanner->start();
#else
    QDir testImages(":/pictures");
    QStringList pics = testImages.entryList();
    for (int i = 0; i < pics.size(); ++i)
    {
        QUrl url;
        url.setScheme("file");
        url.setPath(testImages.filePath(pics.at(i)));
        emit imageUrl(url);
    }
    qDebug() << "== test images loaded ==";
#endif
}

void ImageManager::quit()
{
    QThread::quit();
}

void ImageManager::run()
{
    if (m_url.scheme() != "file")
    {
        qWarning("URL scheme %s not yet supported", qPrintable(m_url.scheme()));
        return;
    }

    // execute once in the event loop below
    QTimer::singleShot(0, this, SLOT(scanForFiles()));

#ifndef QT_NO_THREADED_FILE_LOAD
    ThreadPool pool;

    connect(this, SIGNAL(deployLoader(ThumbnailableImage)),
            &pool, SLOT(deployLoader(ThumbnailableImage)));

    connect(this, SIGNAL(stopAll()), &pool, SLOT(stop()));
    connect(&pool, SIGNAL(stopped()), this, SLOT(quit()));
#endif

    exec();
}
