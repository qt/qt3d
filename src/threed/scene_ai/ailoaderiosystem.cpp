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

#include "ailoaderiosystem_p.h"
#include "ailoaderiostream_p.h"
#include "DefaultLogger.h"

#include <QtCore/qfile.h>
#include <QtCore/qdir.h>

AiLoaderIOSystem::AiLoaderIOSystem(QIODevice *device, QUrl url)
    : m_device(device)
    , m_url(url)
{
}

AiLoaderIOSystem::~AiLoaderIOSystem()
{
    // we don't own m_device
    qDeleteAll(m_sub);
}

bool AiLoaderIOSystem::Exists(const char* path) const
{
    Q_UNUSED(path)
    return m_device->isReadable();
}

char AiLoaderIOSystem::getOsSeparator() const
{
    return QDir::separator().toLatin1();
}

/*!
    \internal
    Open the pFile with the pMode, where mode is given by "man fopen"
*/
Assimp::IOStream* AiLoaderIOSystem::Open(const char* pFile, const char* pMode)
{
    // This is just the file already opened on the device
    if (m_url.toEncoded().endsWith(pFile))
        return new AiLoaderIOStream(m_device);

    // New relative file
    QUrl rel;
    rel.setScheme(m_url.scheme());
    rel.setPath(QLatin1String(pFile));
    QUrl url = m_url.resolved(rel);

    // TODO: handle network case
    if (url.scheme() != QLatin1String("file"))
    {
        //network cases should already be opened by this stage - they
        //depend entirely on the download manager returning a QBuffer, so
        //cannot be handled as the simple "open file" case shown below.
        qWarning("Directly opening a network URL is not supported.  Use scene handler code. (URL: %s).", qPrintable(url.scheme()));
        return 0;
    }

    char mode_str[4];
    memset(mode_str, '\0', 4);
    int i = 0;
    for (const char *ptr = pMode; i < 4 && *ptr; ++ptr)
    {
        if (*ptr != 'b') // ignore the binary attribute
            mode_str[i++] = *ptr;
    }
    QIODevice::OpenMode mode = QIODevice::NotOpen;
    if (::strncmp("r", mode_str, 1) == 0)
    {
        mode = QIODevice::ReadOnly;
    }
    else if (::strncmp("r+", mode_str, 2) == 0)
    {
        mode = QIODevice::ReadWrite;
    }
    else if (::strncmp("w", mode_str, 1) == 0)
    {
        mode = QIODevice::WriteOnly | QIODevice::Truncate;
    }
    else if (::strncmp("w+", mode_str, 2) == 0)
    {
        mode = QIODevice::ReadWrite | QIODevice::Truncate;
    }
    else if (::strncmp("a", mode_str, 1) == 0)
    {
        mode = QIODevice::WriteOnly | QIODevice::Append;
    }
    else if (::strncmp("a+", mode_str, 2) == 0)
    {
        mode = QIODevice::ReadWrite | QIODevice::Append;
    }
    else
    {
        std::string err("Error: invalid mode flag:");
        err.append(mode_str).append(" when opening ").append(pFile);
        Assimp::DefaultLogger::get()->warn(err);
        return 0;
    }

    QFile *f = new QFile(url.toLocalFile());
    bool res = f->open(mode);
    if (!res)
    {
        std::string err("Error: could not open subsequent file:");
        err.append(pFile).append("--").append(f->errorString().toLatin1().constData());
        Assimp::DefaultLogger::get()->warn(err);
        delete f;
        return 0;
    }
    m_sub.append(f);
    AiLoaderIOStream *s = new AiLoaderIOStream(f);
    return s;
}

void AiLoaderIOSystem::Close(Assimp::IOStream* stream)
{
    AiLoaderIOStream *s = static_cast<AiLoaderIOStream*>(stream);
    Q_ASSERT(s);
    s->device()->close();
    delete stream;
}
