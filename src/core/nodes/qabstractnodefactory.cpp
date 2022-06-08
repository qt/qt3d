// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractnodefactory_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*
 * \internal
 * \class Qt3DCore::QAbstractNodeFactory
 * \inheaderfile Qt3DCore/QAbstractNodeFactory
 * \inmodule Qt3DCore
 *
 * \TODO
 */

/*
 * \fn Qt3DCore::QNode * Qt3DCore::QAbstractNodeFactory::createNode(const char *type)
 * Creates a node of \a type.
 */
QAbstractNodeFactory::~QAbstractNodeFactory()
{
}

QList<QAbstractNodeFactory *> node_factories;

/*
 * Registers the given \a factory
 */
void QAbstractNodeFactory::registerNodeFactory(QAbstractNodeFactory *factory)
{
    node_factories << factory;
}

/*
 * \return the node factories.
 */
QList<QAbstractNodeFactory *> QAbstractNodeFactory::nodeFactories()
{
    return node_factories;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
