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

#ifndef SHADERPROGRAM_P_H
#define SHADERPROGRAM_P_H

#include <QtCore/qsharedpointer.h>
#include <QtCore/qpointer.h>
#include <QOpenGLShaderProgram>

#include "qquickeffect.h"
#include "qglshaderprogrameffect.h"

QT_BEGIN_NAMESPACE

class ShaderProgram;
class ShaderProgramEffect;

/*!
  \internal
    This class's purpose is to be a proxy for signals for the main shader
    program.

    This is necessary so the qt_metacall trick can be used to tell apart
    the update signals from QML generated properties.  (In short, each update
    signal is connected to an imaginary slot, but calling the slot is
    intercepted and replaced by a call to the appropriate update function).
*/
class ShaderProgramPropertyListener : public QObject
{
    Q_OBJECT
public:
    ShaderProgramPropertyListener(QObject* parent = 0)
        : QObject(parent)
    {
    }
    virtual ~ShaderProgramPropertyListener()
    {
    }
Q_SIGNALS:
    void propertyChanged();
};

/*!
  \internal
  \sa ShaderProgramPropertyListener
*/
class ShaderProgramPropertyListenerEx : public ShaderProgramPropertyListener
{
public:
    ShaderProgramPropertyListenerEx(ShaderProgram* parent, ShaderProgramEffect* effect);
    ~ShaderProgramPropertyListenerEx();

protected:
    virtual int qt_metacall(QMetaObject::Call c, int id, void **a);
    ShaderProgramEffect* effect;
private:
    int shaderProgramMethodCount;
};


/*!
  \internal
  The ShaderProgramEffect class underlies the ShaderProgram class in Qml/3d.
  It contains the actual QGLShaderProgram along with all of the necessary
  parameters to use that program.
*/
class ShaderProgramEffect : public QGLShaderProgramEffect
{
public:
    ShaderProgramEffect(ShaderProgram* parent);
    virtual ~ShaderProgramEffect();

    bool create(const QString& vertexShader, const QString& fragmentShader);

    void update(QGLPainter *painter, QGLPainter::Updates updates);
    bool setUniformForPropertyIndex(int propertyIndex, QGLPainter *painter);

    void setPropertiesDirty();
    void setPropertyDirty(int property);

    void setAttributeFields(QGL::VertexAttribute fields);
protected:
    void processTextureUrl(int uniformLocation, QString urlString);
    void afterLink();

private:
    void setUniform(int uniformValue, const QImage& image,
                             QGLPainter* painter);
    void setUniform(int uniformValue, const QPixmap pixmap,
                             QGLPainter* painter);
    QGLTexture2D* textureForUniformValue(int uniformLocation);
    int textureUnitForUniformValue(int uniformLocation);

    QPointer<ShaderProgram> parent;
    int nextTextureUnit;
    QMap<int, int> propertyIdsToUniformLocations;
    QMap<int, int> uniformLocationsToTextureUnits;
    QList<int> dirtyProperties;
    QArray<int> propertiesWithoutNotificationSignal;
    ShaderProgramPropertyListener* propertyListener;

    // Thes maps are all referenced by uniform location
    QMap<int, QGLTexture2D*> texture2Ds;
    QMap<int, QImage> images;
    QMap<int, QString> urls;

    // These are sets of uniform locations
    QSet<int> loadingTextures;
    QSet<int> changedTextures;
};

QT_END_NAMESPACE

#endif // SHADERPROGRAM_P_H
