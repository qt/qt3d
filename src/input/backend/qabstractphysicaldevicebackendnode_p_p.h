// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_P_P_H
#define QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_P_P_H

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

#include <QtCore/qlist.h>
#include <Qt3DCore/qnodeid.h>

#include <Qt3DInput/private/movingaverage_p.h>
#include <Qt3DInput/private/qt3dinput_global_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisSetting;
class QInputAspect;

namespace Input {

struct AxisIdSetting
{
    int m_axisIdentifier;
    Qt3DCore::QNodeId m_axisSettingsId;
};

struct AxisIdFilter
{
    int m_axisIdentifier;
    MovingAverage m_filter;
};

class AxisSetting;

}

class Q_3DINPUTSHARED_PRIVATE_EXPORT QAbstractPhysicalDeviceBackendNodePrivate : public Qt3DCore::QBackendNodePrivate
{
public:
    explicit QAbstractPhysicalDeviceBackendNodePrivate(Qt3DCore::QBackendNode::Mode mode = Qt3DCore::QBackendNode::ReadOnly);

    Q_DECLARE_PUBLIC(QAbstractPhysicalDeviceBackendNode)

    void addAxisSetting(int axisIdentifier, Qt3DCore::QNodeId axisSettingId);
    void removeAxisSetting(Qt3DCore::QNodeId axisSettingsId);

    Input::MovingAverage &getOrCreateFilter(int axisIdentifier);

    Input::AxisSetting *getAxisSetting(Qt3DCore::QNodeId axisSettingId) const;

    Qt3DCore::QNodeIdVector m_currentAxisSettingIds;
    QList<Input::AxisIdSetting> m_axisSettings;
    QList<Input::AxisIdFilter> m_axisFilters;
    QInputAspect *m_inputAspect;
};

}

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_P_P_H

