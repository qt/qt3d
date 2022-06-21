// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractaspectjobmanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*
    \internal
    \class Qt3DCore::QAbstractAspectJobManager
    \inmodule Qt3DCore
    \brief A job manager for QAbstractAspect subclasses.
*/
QAbstractAspectJobManager::QAbstractAspectJobManager(QObject *parent)
    : QObject(parent)
{
}

}

QT_END_NAMESPACE

#include "moc_qabstractaspectjobmanager_p.cpp"
