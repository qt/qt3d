/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#include "qskeletoncreatedchange_p.h"
#include "qskeletoncreatedchange_p_p.h"
#include <Qt3DCore/private/qabstractskeleton_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QSkeletonCreatedChangeBasePrivate::QSkeletonCreatedChangeBasePrivate(const QAbstractSkeleton *skeleton)
    : Qt3DCore::QNodeCreatedChangeBasePrivate(skeleton)
    , m_type(QAbstractSkeletonPrivate::get(skeleton)->m_type)
{

}

/*
    \internal
    \class Qt3DCore::QSkeletonCreatedChangeBase
    \inmodule Qt3DCore
    \brief Base class for handling creation changes for QAbstractSkeleton sub-classes.
*/
QSkeletonCreatedChangeBase::QSkeletonCreatedChangeBase(const QAbstractSkeleton *skeleton)
    : Qt3DCore::QNodeCreatedChangeBase(*new QSkeletonCreatedChangeBasePrivate(skeleton), skeleton)
{
}

/*! \internal */
QSkeletonCreatedChangeBase::~QSkeletonCreatedChangeBase()
{
}

/*! \internal */
QSkeletonCreatedChangeBase::SkeletonType QSkeletonCreatedChangeBase::type() const
{
    Q_D(const QSkeletonCreatedChangeBase);
    return d->m_type;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
