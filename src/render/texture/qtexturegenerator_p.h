// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT3DRENDER_QTEXTUREGENERATOR_P_H
#define QT3DRENDER_QTEXTUREGENERATOR_P_H

#include <Qt3DCore/qabstractfunctor.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtexturewrapmode.h>
#include <QtCore/QSharedPointer>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureData;
typedef QSharedPointer<QTextureData> QTextureDataPtr;

class Q_3DRENDERSHARED_EXPORT QTextureGenerator : public Qt3DCore::QAbstractFunctor
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

#endif // QT3DRENDER_QTEXTUREGENERATOR_P_H
