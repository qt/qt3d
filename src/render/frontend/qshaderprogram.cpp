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
#include <Qt3DCore/qscenepropertychange.h>
#include <QDebug>
#include <QFile>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QShaderProgramPrivate::QShaderProgramPrivate(QShaderProgram *qq)
    : QAbstractShaderPrivate(qq)
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
        emit vertexSourceFileChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
            e->setPropertyName(QByteArrayLiteral("vertexSourceFile"));
            e->setValue(vertexSourceFile);
            notifyObservers(e);
        }
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
        emit fragmentSourceFileChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
            e->setPropertyName(QByteArrayLiteral("fragmentSourceFile"));
            e->setValue(fragmentSourceFile);
            notifyObservers(e);
        }
    }
}

QString QShaderProgram::fragmentSourceFile() const
{
    Q_D(const QShaderProgram);
    return d->m_fragmentSourceFile;
}

/*!
 * Sets the vertexShader from raw data in \a vertexShader.
 */
void QShaderProgram::setVertexShader(const QByteArray &vertexShader)
{
    Q_D(QShaderProgram);
    if (vertexShader != d->m_cachedVertexCode) {
        d->m_cachedVertexCode = vertexShader;
        emit vertexShaderChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
            e->setPropertyName(QByteArrayLiteral("vertexSourceCode"));
            e->setValue(d->m_cachedVertexCode);
            notifyObservers(e);
        }
    }
}

/*!
 * Sets the fragmentShader from raw data in \a fragmentShader.
 */
void QShaderProgram::setFragmentShader(const QByteArray &fragmentShader)
{
    Q_D(QShaderProgram);
    if (fragmentShader != d->m_cachedFragmentCode) {
        d->m_cachedFragmentCode = fragmentShader;
        emit fragmentShaderChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, this));
            e->setPropertyName(QByteArrayLiteral("fragmentSourceCode"));
            e->setValue(d->m_cachedFragmentCode);
            notifyObservers(e);
        }
    }
}

QByteArray QShaderProgram::vertexSourceCode() const
{
    Q_D(const QShaderProgram);
    return d->m_cachedVertexCode;
}

QByteArray QShaderProgram::fragmentSourceCode() const
{
    Q_D(const QShaderProgram);
    return d->m_cachedFragmentCode;
}

} // of namespace Qt3D

QT_END_NAMESPACE
