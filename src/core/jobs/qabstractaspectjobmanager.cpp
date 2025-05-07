// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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
