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

#ifndef QGLSHADERPROGRAMEFFECT_H
#define QGLSHADERPROGRAMEFFECT_H

#include <Qt3D/qglabstracteffect.h>

QT_BEGIN_NAMESPACE

class QGLShaderProgramEffectPrivate;
class QOpenGLShaderProgram;

class Q_QT3D_EXPORT QGLShaderProgramEffect : public QGLAbstractEffect
{
public:
    QGLShaderProgramEffect();
    virtual ~QGLShaderProgramEffect();

    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);

    QByteArray vertexShader() const;
    void setVertexShader(const QByteArray &source);
    void setVertexShaderFromFile(const QString &fileName);

    QByteArray geometryShader() const;
    void setGeometryShader(const QByteArray &source);
    void setGeometryShaderFromFile(const QString &fileName);

    QByteArray fragmentShader() const;
    void setFragmentShader(const QByteArray &source);
    void setFragmentShaderFromFile(const QString &fileName);

    int maximumLights() const;
    void setMaximumLights(int value);

    QOpenGLShaderProgram *program() const;

protected:
    virtual bool beforeLink();
    virtual void afterLink();

private:
    QScopedPointer<QGLShaderProgramEffectPrivate> d_ptr;

    Q_DISABLE_COPY(QGLShaderProgramEffect)
    Q_DECLARE_PRIVATE(QGLShaderProgramEffect)
};

QT_END_NAMESPACE

#endif
