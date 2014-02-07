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

#include "ailoaderiostream_p.h"

#include <QtCore/qiodevice.h>

#include <QtCore/qdebug.h>

AiLoaderIOStream::AiLoaderIOStream(QIODevice *device)
    : m_device(device)
    , m_errorState(false)
{
    Q_ASSERT(device);
}

AiLoaderIOStream::~AiLoaderIOStream()
{
    // Nothing to do here
}

size_t AiLoaderIOStream::Read( void* pvBuffer, size_t pSize, size_t pCount)
{
    qint64 result = m_device->read((char*)pvBuffer, pSize * pCount);
    size_t res = result;
    m_errorState = (result == -1);
    if (m_errorState)
        qWarning("AI read error: %s\n", qPrintable(m_device->errorString()));
    if ((pSize * pCount) != res) // AI will treat as error
    {
        long unsigned int pSizeP = (long unsigned int)pSize;
        long unsigned int pCountP = (long unsigned int)pCount;
        long unsigned int pRes = (long unsigned int)res;
        qWarning("read mismatch requested size: %lu x count: %lu = %lu != %lu actuall read\n",
                pSizeP, pCountP, (pSizeP * pCountP), pRes);
    }
    return res;
}

size_t AiLoaderIOStream::Write( const void* pvBuffer, size_t pSize, size_t pCount)
{
    qint64 result = m_device->write((char*)pvBuffer, pSize * pCount);
    m_errorState = (result == -1);
    if (m_errorState)
        qWarning("AI write error: %s\n", qPrintable(m_device->errorString()));
    return result;
}

aiReturn AiLoaderIOStream::Seek(size_t pOffset, aiOrigin pOrigin)
{
    // cannot deal with sockets right now
    Q_ASSERT(!m_device->isSequential());
    switch (pOrigin)
    {
    case aiOrigin_SET:
        m_errorState = m_device->seek(pOffset);
        break;
    case aiOrigin_CUR:
        m_errorState = m_device->seek(m_device->pos() + pOffset);
        break;
    case aiOrigin_END:
        m_errorState = m_device->seek(m_device->size() + pOffset);
        break;
    default:
        Q_ASSERT(0);
        return(aiReturn_FAILURE);
    }
    if (m_errorState)
        return aiReturn_FAILURE;
    return aiReturn_SUCCESS;
}

size_t AiLoaderIOStream::Tell() const
{
    return m_device->pos();
}

size_t AiLoaderIOStream::FileSize() const
{
    return m_device->size();
}

void AiLoaderIOStream::Flush()
{
    // do nothing
}
