/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "qdeclarativeeffect.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class ShaderProgramPrivate;
class QGLSceneNode;

class ShaderProgram : public QDeclarativeEffect
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

QML_DECLARE_TYPE(ShaderProgram)

QT_END_NAMESPACE

QT_END_HEADER

#endif
