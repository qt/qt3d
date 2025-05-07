// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qgeometryloaderfactory_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QGeometryLoaderFactory::QGeometryLoaderFactory(QObject *parent)
    : QObject(parent)
{
}

QGeometryLoaderFactory::~QGeometryLoaderFactory()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qgeometryloaderfactory_p.cpp"

