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

#ifndef QT3D_RENDER_QUICK_QUICK3DTEXTURE_H
#define QT3D_RENDER_QUICK_QUICK3DTEXTURE_H

#include <Qt3DQuickRenderer/qt3dquickrenderer_global.h>
#include <Qt3DQuick/quick3dnode.h>
#include <Qt3DRenderer/qtexture.h>

#include <QQmlListProperty>
#include <QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

class QT3DQUICKRENDERERSHARED_EXPORT Quick3DTextureExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3D::QAbstractTextureImage> textureImages READ textureImages)
    Q_CLASSINFO("DefaultProperty", "textureImages")

public:
    explicit Quick3DTextureExtension(QObject *parent = 0);

    QQmlListProperty<QAbstractTextureImage> textureImages();
    inline QAbstractTextureProvider *parentTexture() const { return qobject_cast<QAbstractTextureProvider *>(parent()); }

private:
    static void appendTextureImage(QQmlListProperty<QAbstractTextureImage> *list, QAbstractTextureImage *textureImage);
    static QAbstractTextureImage *textureImageAt(QQmlListProperty<QAbstractTextureImage> *list, int index);
    static int textureImageCount(QQmlListProperty<QAbstractTextureImage> *list);
    static void clearTextureImageList(QQmlListProperty<QAbstractTextureImage> *list);
};

class QT3DQUICKRENDERERSHARED_EXPORT Quick3DTextureCubeMapExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged())
    Q_PROPERTY(QUrl positiveX READ positiveX WRITE setPositiveX NOTIFY positiveXChanged())
    Q_PROPERTY(QUrl positiveY READ positiveY WRITE setPositiveY NOTIFY positiveYChanged())
    Q_PROPERTY(QUrl positiveZ READ positiveZ WRITE setPositiveZ NOTIFY positiveZChanged())
    Q_PROPERTY(QUrl negativeX READ negativeX WRITE setNegativeX NOTIFY negativeXChanged())
    Q_PROPERTY(QUrl negativeY READ negativeY WRITE setNegativeY NOTIFY negativeYChanged())
    Q_PROPERTY(QUrl negativeZ READ negativeZ WRITE setNegativeZ NOTIFY negativeZChanged())

public:
    explicit Quick3DTextureCubeMapExtension(QObject *parent = 0);

    void setSource(const QUrl &source);
    void setPositiveX(const QUrl &positiveX);
    void setPositiveY(const QUrl &positiveY);
    void setPositiveZ(const QUrl &positiveZ);
    void setNegativeX(const QUrl &negativeX);
    void setNegativeY(const QUrl &negativeY);
    void setNegativeZ(const QUrl &negativeZ);

    QUrl source() const;
    QUrl positiveX() const;
    QUrl positiveY() const;
    QUrl positiveZ() const;
    QUrl negativeX() const;
    QUrl negativeY() const;
    QUrl negativeZ() const;

Q_SIGNALS:
    void positiveXChanged();
    void positiveYChanged();
    void positiveZChanged();
    void negativeXChanged();
    void negativeYChanged();
    void negativeZChanged();
    void sourceChanged();

private:
    QUrl m_positiveX;
    QUrl m_positiveY;
    QUrl m_positiveZ;
    QUrl m_negativeX;
    QUrl m_negativeY;
    QUrl m_negativeZ;
    QUrl m_source;

    inline QTextureCubeMap *parentTexture() { return qobject_cast<QTextureCubeMap *>(parent()); }
    void loadFace(const QUrl &faceUrl, QAbstractTextureProvider::CubeMapFace face);
};

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QUICK3DTEXTURE_H
