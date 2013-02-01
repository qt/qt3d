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

#ifndef QGLMATERIAL_H
#define QGLMATERIAL_H

#include <Qt3D/qglabstractmaterial.h>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>

#include <QtGui/qcolor.h>

QT_BEGIN_NAMESPACE

class QGLMaterialPrivate;
class QGLTexture2D;
class QGLTextureCube;
class QGLPainter;
class QGLMaterialCollection;
class QGLTwoSidedMaterial;

class Q_QT3D_EXPORT QGLMaterial : public QGLAbstractMaterial
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLMaterial)
    Q_DISABLE_COPY(QGLMaterial)
    Q_ENUMS(TextureCombineMode)
    Q_PROPERTY(QColor ambientColor READ ambientColor WRITE setAmbientColor NOTIFY ambientColorChanged)
    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)
    Q_PROPERTY(QColor specularColor READ specularColor WRITE setSpecularColor NOTIFY specularColorChanged)
    Q_PROPERTY(QColor emittedLight READ emittedLight WRITE setEmittedLight NOTIFY emittedLightChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(QGLTexture2D *texture READ texture WRITE setTexture NOTIFY texturesChanged)
    Q_PROPERTY(QGLMaterial::TextureCombineMode textureCombineMode READ textureCombineMode WRITE setTextureCombineMode NOTIFY texturesChanged)
    Q_PROPERTY(QUrl textureUrl READ textureUrl WRITE setTextureUrl NOTIFY texturesChanged)
public:
    explicit QGLMaterial(QObject *parent = 0);
    ~QGLMaterial();

    QColor ambientColor() const;
    void setAmbientColor(const QColor& value);

    QColor diffuseColor() const;
    void setDiffuseColor(const QColor& value);

    QColor specularColor() const;
    void setSpecularColor(const QColor& value);

    QColor emittedLight() const;
    void setEmittedLight(const QColor& value);

    void setColor(const QColor& value);

    float shininess() const;
    void setShininess(float value);

    QGLTexture2D *texture(int layer = 0) const;
    void setTexture(QGLTexture2D *value, int layer = 0);

    QUrl textureUrl(int layer = 0) const;
    void setTextureUrl(const QUrl &url, int layer = 0);

    enum TextureCombineMode
    {
        Modulate,
        Decal,
        Replace
    };

    QGLMaterial::TextureCombineMode textureCombineMode(int layer = 0) const;
    void setTextureCombineMode(QGLMaterial::TextureCombineMode mode, int layer = 0);

    int textureLayerCount() const;

    QGLMaterial *front() const;
    bool isTransparent() const;
    void bind(QGLPainter *painter);
    void release(QGLPainter *painter, QGLAbstractMaterial *next);
    void prepareToDraw(QGLPainter *painter, const QGLAttributeSet &attributes);

Q_SIGNALS:
    void ambientColorChanged();
    void diffuseColorChanged();
    void specularColorChanged();
    void emittedLightChanged();
    void shininessChanged();
    void texturesChanged();

private:
    friend class QGLMaterialCollection;
    friend class QGLTwoSidedMaterial;

    void bindTextures(QGLPainter *painter);
    void bindEffect(QGLPainter *painter, const QGLAttributeSet &attributes, bool twoSided);

    QScopedPointer<QGLMaterialPrivate> d_ptr;
};

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLMaterial &material);
#endif

QT_END_NAMESPACE

#endif
