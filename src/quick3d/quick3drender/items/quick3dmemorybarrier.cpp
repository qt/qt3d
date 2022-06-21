// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
