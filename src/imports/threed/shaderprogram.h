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

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "qquickeffect.h"

QT_BEGIN_NAMESPACE

class ShaderProgramPrivate;
class QGLSceneNode;

class ShaderProgram : public QQuickEffect
{
    Q_OBJECT
    Q_PROPERTY(QString vertexShader READ vertexShader WRITE setVertexShader NOTIFY shaderChanged)
    Q_PROPERTY(QString fragmentShader READ fragmentShader WRITE setFragmentShader NOTIFY shaderChanged)
public:
    ShaderProgram(QObject *parent = 0);
    virtual ~ShaderProgram();

    QString vertexShader() const;
    void setVertexShader(const QString& value);

    QString fragmentShader() const;
    void setFragmentShader(const QString& value);

    virtual void enableEffect(QGLPainter *painter);
    virtual void applyTo(QGLSceneNode *node);
public Q_SLOTS:
    void markAllPropertiesDirty();
    void markPropertyDirty(int property);
Q_SIGNALS:
    void finishedLoading();
    void shaderChanged();
private:
    ShaderProgramPrivate *d;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(ShaderProgram)

#endif
