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

#ifndef QT3DRENDER_QTEXTUREGENERATOR_H
#define QT3DRENDER_QTEXTUREGENERATOR_H

#include <Qt3DRender/qabstractfunctor.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtexturewrapmode.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureData;
typedef QSharedPointer<QTextureData> QTextureDataPtr;

class Q_3DRENDERSHARED_EXPORT QTextureGenerator : public QAbstractFunctor
{
public:
    virtual ~QTextureGenerator();
    virtual QTextureDataPtr operator()() = 0;
    virtual bool operator ==(const QTextureGenerator &other) const = 0;
};

typedef QSharedPointer<QTextureGenerator> QTextureGeneratorPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QTextureGeneratorPtr) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QTEXTUREGENERATOR_H
