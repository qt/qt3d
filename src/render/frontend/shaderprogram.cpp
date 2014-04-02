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

QT_BEGIN_NAMESPACE

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

/*!
 * Sets the vertexShader from raw data in \a vertexShader.
 * Note that if vertexSourceFile is set, when load is called,
 * the shader code will be replaced by the shader located at vertexSourceFile.
 */
void ShaderProgram::setVertexShader(const QByteArray &vertexShader)
{
    if (vertexShader != m_cachedVertexCode) {
        m_cachedVertexCode = vertexShader;
        m_isLoaded = false;
        emit vertexShaderChanged();
    }
}

/*!
 * Sets the fragmentShader from raw data in \a fragmentShader.
 * Note that if a fragmentSourceFile is set, when load is called,
 * the shader code will be replaced by the shader located at fragmentSourceFile.
 */
void ShaderProgram::setFragmentShader(const QByteArray &fragmentShader)
{
    if (fragmentShader != m_cachedFragmentCode) {
        m_cachedFragmentCode = fragmentShader;
        m_isLoaded = false;
        emit fragmentShaderChanged();
    }
}

QByteArray ShaderProgram::vertexSourceCode() const
{
    if (!isLoaded() && m_cachedVertexCode.isEmpty())
        return QByteArray();

    return m_cachedVertexCode;
}

QByteArray ShaderProgram::fragmentSourceCode() const
{
    if (!isLoaded() && m_cachedFragmentCode.isEmpty())
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

    if (!m_fragmentSourceFile.isEmpty()) {
        QFile f(m_fragmentSourceFile);
        if (!f.exists()) {
            qWarning() << "couldn't find shader source file:" << m_fragmentSourceFile;
            return;
        } else {
            f.open(QIODevice::ReadOnly);
            m_cachedFragmentCode = f.readAll();
        }
    }
    if (!m_vertexSourceFile.isEmpty()) {
        QFile vs(m_vertexSourceFile);
        if (!vs.exists()) {
            qWarning() << "couldn't find shader source file:" << m_vertexSourceFile;
            return;
        } else {
            vs.open(QIODevice::ReadOnly);
            m_cachedVertexCode = vs.readAll();
        }
    }
}

} // of namespace Qt3D

QT_END_NAMESPACE
