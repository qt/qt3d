/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "launcher.h"
#include "imageloader.h"
#include "imagemanager.h"

#include <QSemaphore>
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QSet>
#include <QDebug>

Launcher::Launcher(ImageManager *manager)
    : m_manager(manager)
    , m_stop(false)
{
    Q_ASSERT(m_manager);
}

void Launcher::run()
{
    if (m_url.isEmpty())
    {
        qWarning("Launcher::run - empty URL!");
        return;
    }
    if (m_url.scheme() != QLatin1String("file"))
    {
        qWarning("URL scheme %s not yet supported", qPrintable(m_url.scheme()));
        return;
    }
    QStringList queue;
    queue.append(m_url.path());
    QSet<QString> loopProtect;
    while (queue.size() > 0 && !m_stop)
    {
        QString path = queue.takeFirst();
        QFileInfo u(path);
        if (u.isSymLink())
            path = u.symLinkTarget();
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
                    if ((*it).startsWith(QLatin1Char('.')))
                        continue;
                    queue.append(dir.absoluteFilePath(*it));
                }
            }
        }
        else
        {
            if (u.isFile() && u.isReadable())
            {
                // do no checking here for file extensions etc - just
                // forward any readable file found under the pictures
                // directory to the QImage loader, and let it sort out
                // if the thing can be loaded as an image.
                QUrl url2;
                url2.setScheme(QLatin1String("file"));
                url2.setPath(u.absoluteFilePath());
                m_manager->acquire();
                emit imageUrl(url2);
            }
        }
    }
}
