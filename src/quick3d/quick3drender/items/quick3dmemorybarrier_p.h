// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DMEMORYBARRIER_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DMEMORYBARRIER_P_H

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

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DRender/qmemorybarrier.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

// Qt6: Handle QFlags as Q_PROPERTY in QML

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DMemoryBarrier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int waitFor READ waitFor WRITE setWaitFor)
public:
    explicit Quick3DMemoryBarrier(QObject *parent = nullptr);
    ~Quick3DMemoryBarrier();

    inline QMemoryBarrier *parentBarrier() const { return qobject_cast<QMemoryBarrier*>(parent()); }

    void setWaitFor(int waitOperations);
    int waitFor() const;
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DMEMORYBARRIER_P_H
