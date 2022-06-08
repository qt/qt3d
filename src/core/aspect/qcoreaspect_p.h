// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QCOREASPECT_P_H
#define QT3DCORE_QCOREASPECT_P_H

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

#include <Qt3DCore/qcoreaspect.h>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class CalculateBoundingVolumeJob;
using CalculateBoundingVolumeJobPtr = QSharedPointer<CalculateBoundingVolumeJob>;

class Q_3DCORE_PRIVATE_EXPORT QCoreAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
public:
    QCoreAspectPrivate();
    ~QCoreAspectPrivate();

    Q_DECLARE_PUBLIC(QCoreAspect)
    static QCoreAspectPrivate *get(QCoreAspect *aspect);

    void jobsDone() override;
    void frameDone() override;

    bool m_initialized;
    CalculateBoundingVolumeJobPtr m_calculateBoundingVolumeJob;
    bool m_boundingVolumesEnabled;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QCOREASPECT_P_H
