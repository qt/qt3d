/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef ASSIMPHELPERS_H
#define ASSIMPHELPERS_H

// ASSIMP INCLUDES
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

#include <QIODevice>
#include <QMap>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace AssimpHelper {

//CUSTOM FILE STREAM
class AssimpIOStream : public Assimp::IOStream
{
public :
    AssimpIOStream(QIODevice *device);
    ~AssimpIOStream();

    size_t Read(void *pvBuffer, size_t pSize, size_t pCount) Q_DECL_OVERRIDE;
    size_t Write(const void *pvBuffer, size_t pSize, size_t pCount) Q_DECL_OVERRIDE;
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) Q_DECL_OVERRIDE;
    size_t Tell() const Q_DECL_OVERRIDE;
    size_t FileSize() const ;
    void Flush() Q_DECL_OVERRIDE;

private:
    QIODevice *const m_device;
};

//CUSTOM FILE IMPORTER TO HANDLE QT RESOURCES WITHIN ASSIMP
class AssimpIOSystem : public Assimp::IOSystem
{
public :
    AssimpIOSystem();
    ~AssimpIOSystem();
    bool Exists(const char *pFile) const Q_DECL_OVERRIDE;
    char getOsSeparator() const Q_DECL_OVERRIDE;
    Assimp::IOStream *Open(const char *pFile, const char *pMode) Q_DECL_OVERRIDE;
    void Close(Assimp::IOStream *pFile) Q_DECL_OVERRIDE;

private:
    QMap<QByteArray, QIODevice::OpenMode> m_openModeMaps;
};

} // AssimpHelper namespace
} // Qt3D namespace

QT_END_NAMESPACE

#endif // ASSIMPHELPERS_H
