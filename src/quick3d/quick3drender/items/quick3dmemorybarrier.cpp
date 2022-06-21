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

#include "quick3dmemorybarrier_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {


Quick3DMemoryBarrier::Quick3DMemoryBarrier(QObject *parent)
    : QObject(parent)
{
}

Quick3DMemoryBarrier::~Quick3DMemoryBarrier()
{
}

void Quick3DMemoryBarrier::setWaitFor(int barrierTypes)
{
    QMemoryBarrier::Operations types(QMemoryBarrier::All);
    types &= barrierTypes; // Will only keep flags that are actually set
    parentBarrier()->setWaitOperations(types);
}

int Quick3DMemoryBarrier::waitFor() const
{
    return QMemoryBarrier::Operations::Int(parentBarrier()->waitOperations());
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dmemorybarrier_p.cpp"
