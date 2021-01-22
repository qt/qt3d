/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H
#define QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H

#include <Qt3DRender/qabstractfunctor.h>
#include <QtCore/QSharedPointer>

// TO DO TexImageDataPtr -> QImageDataPtr + d_ptr
// We might also get rid of the layer, face, mipmap level from
// TexImageDataPtr and store that in the functor directly
// or use the QTextureImage instead

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImageData;
typedef QSharedPointer<QTextureImageData> QTextureImageDataPtr;

class Q_3DRENDERSHARED_EXPORT QTextureImageDataGenerator : public QAbstractFunctor
{
public:
    virtual ~QTextureImageDataGenerator() {}
    virtual QTextureImageDataPtr operator()() = 0;
    virtual bool operator ==(const QTextureImageDataGenerator &other) const = 0;
};

typedef QSharedPointer<QTextureImageDataGenerator> QTextureImageDataGeneratorPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QTextureImageDataGeneratorPtr) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H
