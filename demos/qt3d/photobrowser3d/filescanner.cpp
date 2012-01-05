/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#include "filescanner.h"

#include <QTime>
#include <QFileInfo>
#include <QStringList>
#include <QSet>
#include <QDir>
#include <QCoreApplication>
#include <QImageReader>
#include <QDebug>

FileScanner::FileScanner(QObject *parent)
    : QThread(parent), m_stop(0)
{ }

FileScanner::~FileScanner()
{
    // nothing to do here
}

void FileScanner::stop()
{
    m_stop.ref();
}

void FileScanner::run()
{
    scan();
}

void FileScanner::scan()
{
    QStringList queue;
    queue.append(m_url.path());
    QSet<QString> loopProtect;
    int count = 0;
    while (queue.size() > 0 && !m_stop.load() && count < 300)
    {
        QString path = queue.takeFirst();
        QFileInfo u(path);
        if (u.isSymLink())
        {
            path = u.symLinkTarget();
            u = QFileInfo(path);
        }
        if (u.isDir())
        {
            if (!loopProtect.contains(path))
            {
                loopProtect.insert(path);
                QDir dir(path);
                QStringList entries = dir.entryList();
                QStringList::const_iterator it = entries.constBegin();
                for ( ; it != entries.constEnd(); ++it)
                {
                    // ignore hidden files, system directories
                    if ((*it).startsWith("."))
                        continue;
                    queue.append(dir.absoluteFilePath(*it));
                }
            }
        }
        else
        {
            if (u.isFile() && u.isReadable())
            {
                // small optimization: if the file has a suffix, check if that
                // is known as an image format before sending to loader
                QString ext = u.suffix().toLower();
                if (ext.isEmpty() ||
                        QImageReader::supportedImageFormats().contains(ext.toLocal8Bit()))
                {
                    QUrl url2;
                    url2.setScheme("file");
                    url2.setPath(u.absoluteFilePath());
                    emit imageUrl(url2);
                    ++count;
                }
            }
        }
        QCoreApplication::processEvents();
        QThread::yieldCurrentThread();
    }
}
