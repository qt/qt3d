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

#ifndef QDECLARATIVEEFFECT_P_H
#define QDECLARATIVEEFFECT_P_H

#include "qt3dquickglobal.h"

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qcolor.h>
#include <QtGui/qimage.h>
#include <QtDeclarative/qdeclarative.h>

#include <Qt3D/qgltexture2d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeEffectPrivate;
class QGLPainter;
class QGLMaterial;

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

    virtual qreal progress();

Q_SIGNALS:
    void effectChanged();
    void progressChanged(qreal progress);

protected Q_SLOTS:
    virtual void textureRequestFinished();
    virtual void textureRequestProgress(qint64 recieived, qint64 total);
protected:
    QGLTexture2D *texture2D();

private:
    QDeclarativeEffectPrivate *d;
};

QML_DECLARE_TYPE(QDeclarativeEffect)

QT_END_NAMESPACE

QT_END_HEADER

#endif
