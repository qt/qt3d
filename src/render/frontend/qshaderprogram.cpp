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

#include "qshaderprogram.h"
#include "qshaderprogram_p.h"

#include <QDebug>
#include <QFile>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QShaderProgramPrivate::QShaderProgramPrivate(QShaderProgram *qq)
    : QAbstractShaderPrivate(qq)
    , m_isLoaded(false)
{
}

QShaderProgram::QShaderProgram(QNode *parent)
    : QAbstractShader(*new QShaderProgramPrivate(this), parent)
{
}

void QShaderProgram::copy(const QNode *ref)
{
    Q_D(QShaderProgram);
    QAbstractShader::copy(ref);
    const QShaderProgram *prog = qobject_cast<const QShaderProgram *>(ref);
    if (prog != Q_NULLPTR) {
        // TO DO : Move loading to the backend
        d->m_vertexSourceFile = prog->vertexSourceFile();
        d->m_isLoaded = prog->isLoaded();
        d->m_fragmentSourceFile = prog->fragmentSourceFile();
        d->m_cachedFragmentCode = prog->d_func()->m_cachedFragmentCode;
        d->m_cachedVertexCode = prog->d_func()->m_cachedVertexCode;
    }
}

QShaderProgram::QShaderProgram(QShaderProgramPrivate &dd, QNode *parent)
    : QAbstractShader(dd, parent)
{
}

QShaderProgram *QShaderProgram::doClone(QNode *clonedParent) const
{
    return new QShaderProgram(clonedParent);
}

void QShaderProgram::setVertexSourceFile(const QString& vertexSourceFile)
{
    Q_D(QShaderProgram);
    if (vertexSourceFile != d->m_vertexSourceFile) {
        d->m_vertexSourceFile = vertexSourceFile;
        d->m_isLoaded = false;
        emit vertexSourceFileChanged();
    }
}

QString QShaderProgram::vertexSourceFile() const
{
    Q_D(const QShaderProgram);
    return d->m_vertexSourceFile;
}

void QShaderProgram::setFragmentSourceFile(const QString& fragmentSourceFile)
{
    Q_D(QShaderProgram);
    if (fragmentSourceFile != d->m_fragmentSourceFile) {
        d->m_fragmentSourceFile = fragmentSourceFile;
        d->m_isLoaded = false;
        emit fragmentSourceFileChanged();
    }
}

QString QShaderProgram::fragmentSourceFile() const
{
    Q_D(const QShaderProgram);
    return d->m_fragmentSourceFile;
}

/*!
 * Sets the vertexShader from raw data in \a vertexShader.
 * Note that if vertexSourceFile is set, when load is called,
 * the shader code will be replaced by the shader located at vertexSourceFile.
 */
void QShaderProgram::setVertexShader(const QByteArray &vertexShader)
{
    Q_D(QShaderProgram);
    if (vertexShader != d->m_cachedVertexCode) {
        d->m_cachedVertexCode = vertexShader;
        d->m_isLoaded = false;
        emit vertexShaderChanged();
    }
}

/*!
 * Sets the fragmentShader from raw data in \a fragmentShader.
 * Note that if a fragmentSourceFile is set, when load is called,
 * the shader code will be replaced by the shader located at fragmentSourceFile.
 */
void QShaderProgram::setFragmentShader(const QByteArray &fragmentShader)
{
    Q_D(QShaderProgram);
    if (fragmentShader != d->m_cachedFragmentCode) {
        d->m_cachedFragmentCode = fragmentShader;
        d->m_isLoaded = false;
        emit fragmentShaderChanged();
    }
}

QByteArray QShaderProgram::vertexSourceCode() const
{
    Q_D(const QShaderProgram);
    if (!isLoaded() && d->m_cachedVertexCode.isEmpty())
        return QByteArray();

    return d->m_cachedVertexCode;
}

QByteArray QShaderProgram::fragmentSourceCode() const
{
    Q_D(const QShaderProgram);
    if (!isLoaded() && d->m_cachedFragmentCode.isEmpty())
        return QByteArray();

    return d->m_cachedFragmentCode;
}

bool QShaderProgram::isLoaded() const
{
    Q_D(const QShaderProgram);
    return d->m_isLoaded;
}

void QShaderProgram::load()
{
    Q_D(QShaderProgram);
    if (d->m_isLoaded)
        return;

    d->m_isLoaded = true;

    if (!d->m_fragmentSourceFile.isEmpty()) {
        QFile f(d->m_fragmentSourceFile);
        if (!f.exists()) {
            qWarning() << "couldn't find shader source file:" << d->m_fragmentSourceFile;
            return;
        } else {
            f.open(QIODevice::ReadOnly);
            d->m_cachedFragmentCode = f.readAll();
        }
    }
    if (!d->m_vertexSourceFile.isEmpty()) {
        QFile vs(d->m_vertexSourceFile);
        if (!vs.exists()) {
            qWarning() << "couldn't find shader source file:" << d->m_vertexSourceFile;
            return;
        } else {
            vs.open(QIODevice::ReadOnly);
            d->m_cachedVertexCode = vs.readAll();
        }
    }
}

} // of namespace Qt3D

QT_END_NAMESPACE
