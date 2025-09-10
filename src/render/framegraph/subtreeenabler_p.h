// Copyright (C) 2019 Ford Motor Company
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef SUBTREEENABLER_P_H
#define SUBTREEENABLER_P_H

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

#include <Qt3DRender/qsubtreeenabler.h>
#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_AUTOTEST_EXPORT SubtreeEnabler : public FrameGraphNode
{
public:
    SubtreeEnabler();
    QSubtreeEnabler::Enablement enablement() const { return m_enablement; }
    void sendDisableToFrontend();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QSubtreeEnabler::Enablement m_enablement = QSubtreeEnabler::Persistent;
};

} //Render

} //Qt3DRender

QT_END_NAMESPACE

#endif // SUBTREEENABLER_P_H
