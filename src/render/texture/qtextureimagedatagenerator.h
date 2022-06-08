// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H
#define QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H

#include <Qt3DCore/qabstractfunctor.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QSharedPointer>

// TO DO TexImageDataPtr -> QImageDataPtr + d_ptr
// We might also get rid of the layer, face, mipmap level from
// TexImageDataPtr and store that in the functor directly
// or use the QTextureImage instead

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImageData;
typedef QSharedPointer<QTextureImageData> QTextureImageDataPtr;

class Q_3DRENDERSHARED_EXPORT QTextureImageDataGenerator : public Qt3DCore::QAbstractFunctor
{
public:
    virtual ~QTextureImageDataGenerator();
    virtual QTextureImageDataPtr operator()() = 0;
    virtual bool operator ==(const QTextureImageDataGenerator &other) const = 0;
};

typedef QSharedPointer<QTextureImageDataGenerator> QTextureImageDataGeneratorPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QTextureImageDataGeneratorPtr) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QTEXTUREIMAGEDATAGENERATOR_H
