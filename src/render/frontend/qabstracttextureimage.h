/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QABSTRACTTEXTUREIMAGE_H
#define QT3D_QABSTRACTTEXTUREIMAGE_H

#include <Qt3DRenderer/qabstracttextureprovider.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

// TO DO TexImageDataPtr -> QImageDataPtr + d_ptr
// We might also get rid of the layer, face, mipmap level from
// TexImageDataPtr and store that in the functor directly
// or use the QTextureImage instead
class QT3DRENDERERSHARED_EXPORT QTextureDataFunctor
{
public:
    virtual ~QTextureDataFunctor() {}
    virtual TexImageDataPtr operator()() = 0;
    virtual bool operator ==(const QTextureDataFunctor &other) const = 0;
};

typedef QSharedPointer<QTextureDataFunctor> QTextureDataFunctorPtr;

class QAbstractTextureImagePrivate;

class QT3DRENDERERSHARED_EXPORT QAbstractTextureImage : public QNode
{
    Q_OBJECT
    Q_PROPERTY(int mipmapLevel READ mipmapLevel WRITE setMipmapLevel NOTIFY mipmapLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Qt3D::QAbstractTextureProvider::CubeMapFace cubeMapFace READ cubeMapFace WRITE setCubeMapFace NOTIFY cubeMapFaceChanged)
public:
    explicit QAbstractTextureImage(QNode *parent = 0);
    virtual ~QAbstractTextureImage();

    int mipmapLevel() const;
    int layer() const;
    QAbstractTextureProvider::CubeMapFace cubeMapFace() const;

    void setMipmapLevel(int level);
    void setLayer(int layer);
    void setCubeMapFace(QAbstractTextureProvider::CubeMapFace face);

    virtual QTextureDataFunctorPtr dataFunctor() const = 0;

Q_SIGNALS:
    void mipmapLevelChanged();
    void layerChanged();
    void cubeMapFaceChanged();

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;
    QAbstractTextureImage(QAbstractTextureImagePrivate &dd, QNode *parent = 0);

private:
    Q_DECLARE_PRIVATE(QAbstractTextureImage)
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QTextureDataFunctorPtr)

#endif // QT3D_QABSTRACTTEXTUREIMAGE_H
