/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QABSTRACTTEXTUREIMAGE_H
#define QT3DRENDER_QABSTRACTTEXTUREIMAGE_H

#include <Qt3DRender/qabstractfunctor.h>
#include <Qt3DRender/qabstracttextureprovider.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

// TO DO TexImageDataPtr -> QImageDataPtr + d_ptr
// We might also get rid of the layer, face, mipmap level from
// TexImageDataPtr and store that in the functor directly
// or use the QTextureImage instead
class QT3DRENDERSHARED_EXPORT QTextureDataFunctor : public QAbstractFunctor
{
public:
    virtual ~QTextureDataFunctor() {}
    virtual QTexImageDataPtr operator()() = 0;
    virtual bool operator ==(const QTextureDataFunctor &other) const = 0;
};

typedef QSharedPointer<QTextureDataFunctor> QTextureDataFunctorPtr;

class QAbstractTextureImagePrivate;

class QT3DRENDERSHARED_EXPORT QAbstractTextureImage : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(int mipmapLevel READ mipmapLevel WRITE setMipmapLevel NOTIFY mipmapLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTextureProvider::CubeMapFace cubeMapFace READ cubeMapFace WRITE setCubeMapFace NOTIFY cubeMapFaceChanged)
public:
    explicit QAbstractTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR);
    virtual ~QAbstractTextureImage();

    int mipmapLevel() const;
    int layer() const;
    QAbstractTextureProvider::CubeMapFace cubeMapFace() const;

    void update();
    virtual QTextureDataFunctorPtr dataFunctor() const = 0;

public Q_SLOTS:
    void setMipmapLevel(int level);
    void setLayer(int layer);
    void setCubeMapFace(QAbstractTextureProvider::CubeMapFace face);

Q_SIGNALS:
    void mipmapLevelChanged(int mipmapLevel);
    void layerChanged(int layer);
    void cubeMapFaceChanged(QAbstractTextureProvider::CubeMapFace cubeMapFace);

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    QAbstractTextureImage(QAbstractTextureImagePrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);

private:
    Q_DECLARE_PRIVATE(QAbstractTextureImage)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QTextureDataFunctorPtr)

#endif // QT3DRENDER_QABSTRACTTEXTUREIMAGE_H
