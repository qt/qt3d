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

#ifndef QQUICKEFFECT_P_H
#define QQUICKEFFECT_P_H

#include <Qt3DQuick/qt3dquickglobal.h>
#include <Qt3D/qgltexture2d.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qcolor.h>
#include <QtGui/qimage.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QQuickEffectPrivate;
class QGLPainter;
class QGLMaterial;
class QGLSceneNode;

class Q_QT3D_QUICK_EXPORT QQuickEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY effectChanged)
    Q_PROPERTY(bool useLighting READ useLighting WRITE setUseLighting NOTIFY effectChanged)
    Q_PROPERTY(bool decal READ decal WRITE setDecal NOTIFY effectChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY effectChanged)
    Q_PROPERTY(QUrl texture READ texture WRITE setTexture NOTIFY effectChanged)
    Q_PROPERTY(QImage textureImage READ textureImage WRITE setTextureImage NOTIFY effectChanged)
    Q_PROPERTY(QGLMaterial *material READ material WRITE setMaterial NOTIFY effectChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
public:
    QQuickEffect(QObject *parent = 0);
    virtual ~QQuickEffect();

    virtual QColor color() const;
    virtual void setColor(const QColor& value);

    virtual bool useLighting() const;
    virtual void setUseLighting(bool value);

    virtual bool decal() const;
    virtual void setDecal(bool value);

    virtual bool blending() const;
    virtual void setBlending(bool value);

    virtual QUrl texture() const;
    virtual void setTexture(const QUrl& value);

    virtual QImage textureImage() const;
    virtual void setTextureImage(const QImage& value);

    virtual QGLMaterial *material() const;
    virtual void setMaterial(QGLMaterial *value);

    virtual void enableEffect(QGLPainter *painter);
    virtual void disableEffect(QGLPainter *painter);
    virtual void applyTo(QGLSceneNode *node);

    virtual float progress();

    void openglContextIsAboutToBeDestroyed();

Q_SIGNALS:
    void effectChanged();
    void progressChanged(float progress);

protected:
    QGLTexture2D *texture2D();
    inline void ensureMaterial();

private:
    QQuickEffectPrivate *d;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickEffect)

#endif
