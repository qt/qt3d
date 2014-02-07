/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "shaderprogram.h"

#include <QDebug>
#include <QFile>

namespace Qt3D {

ShaderProgram::ShaderProgram(Node *parent)
    : Node(parent)
    , m_vertexSourceFile()
    , m_fragmentSourceFile()
    , m_isLoaded(false)
{
}

void ShaderProgram::setVertexSourceFile(const QString& vertexSourceFile)
{
    if (vertexSourceFile != m_vertexSourceFile) {
        m_vertexSourceFile = vertexSourceFile;
        m_isLoaded = false;
        emit vertexSourceFileChanged();
    }
}

QString ShaderProgram::vertexSourceFile() const
{
    return m_vertexSourceFile;
}

void ShaderProgram::setFragmentSourceFile(const QString& fragmentSourceFile)
{
    if (fragmentSourceFile != m_fragmentSourceFile) {
        m_fragmentSourceFile = fragmentSourceFile;
        m_isLoaded = false;
        emit fragmentSourceFileChanged();
    }
}

QString ShaderProgram::fragmentSourceFile() const
{
    return m_fragmentSourceFile;
}

QByteArray ShaderProgram::vertexSourceCode() const
{
    if (!isLoaded())
        return QByteArray();

    return m_cachedVertexCode;
}

QByteArray ShaderProgram::fragmentSourceCode() const
{
    if (!isLoaded())
        return QByteArray();

    return m_cachedFragmentCode;
}

bool ShaderProgram::isLoaded() const
{
    return m_isLoaded;
}

void ShaderProgram::load()
{
    if (m_isLoaded)
        return;

    m_isLoaded = true;

    QFile f(m_fragmentSourceFile);
    if (!f.exists()) {
        qWarning() << "couldn't find shader source file:" << m_fragmentSourceFile;
        return;
    } else {
        f.open(QIODevice::ReadOnly);
        m_cachedFragmentCode = f.readAll();
    }

    QFile vs(m_vertexSourceFile);
    if (!vs.exists()) {
        qWarning() << "couldn't find shader source file:" << m_vertexSourceFile;
        return;
    } else {
        vs.open(QIODevice::ReadOnly);
        m_cachedVertexCode = vs.readAll();
    }
}

} // of namespace Qt3D

