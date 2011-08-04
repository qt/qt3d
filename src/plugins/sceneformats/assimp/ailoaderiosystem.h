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

#ifndef AILOADERIOSYSTEM_H
#define AILOADERIOSYSTEM_H

#include "IOSystem.h"

#include <QtCore/qurl.h>

class QIODevice;
class IOStream;

class AiLoaderIOSystem : public Assimp::IOSystem
{
public:
    AiLoaderIOSystem(QIODevice *device, QUrl url);
    ~AiLoaderIOSystem();
    virtual bool Exists( const char* path) const;
    virtual char getOsSeparator() const;
    virtual Assimp::IOStream* Open(const char* pFile, const char* pMode = "rb");
    virtual void Close(Assimp::IOStream* pFile);
private:
    QIODevice *m_device;
    QList<QIODevice*> m_sub;
    QUrl m_url;
};

#endif // AILOADERIOSYSTEM_H
