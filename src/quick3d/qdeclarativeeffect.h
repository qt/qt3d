/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEEFFECT_P_H
#define QDECLARATIVEEFFECT_P_H

#include "qt3dquickglobal.h"

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qcolor.h>
#include <QtGui/qimage.h>
#include <QtDeclarative/qdeclarative.h>

#include "qgltexture2d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3DQuick)

class QDeclarativeEffectPrivate;
class QGLPainter;
class QGLMaterial;
class QGLSceneNode;

class Q_QT3D_QUICK_EXPORT QDeclarativeEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY effectChanged)
    Q_PROPERTY(bool useLighting READ useLighting WRITE setUseLighting NOTIFY effectChanged)
    Q_PROPERTY(bool decal READ decal WRITE setDecal NOTIFY effectChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY effectChanged)
    Q_PROPERTY(QUrl texture READ texture WRITE setTexture NOTIFY effectChanged)
    Q_PROPERTY(QImage textureImage READ textureImage WRITE setTextureImage NOTIFY effectChanged)
    Q_PROPERTY(QGLMaterial *material READ material WRITE setMaterial NOTIFY effectChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
public:
    QDeclarativeEffect(QObject *parent = 0);
    ~QDeclarativeEffect();

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

    virtual qreal progress();

Q_SIGNALS:
    void effectChanged();
    void progressChanged(qreal progress);

protected:
    QGLTexture2D *texture2D();
    inline void ensureMaterial();

private:
    QDeclarativeEffectPrivate *d;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeEffect)

QT_END_HEADER

#endif
