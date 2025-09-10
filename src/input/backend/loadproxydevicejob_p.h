// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DINPUT_INPUT_LOADPROXYDEVICEJOB_P_H
#define QT3DINPUT_INPUT_LOADPROXYDEVICEJOB_P_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class InputHandler;
class LoadProxyDeviceJobPrivate;

class Q_AUTOTEST_EXPORT LoadProxyDeviceJob : public Qt3DCore::QAspectJob
{
public:
    LoadProxyDeviceJob();
    ~LoadProxyDeviceJob();

    void setProxiesToLoad(const QList<Qt3DCore::QNodeId> &proxies);
    void setInputHandler(InputHandler *handler);

    InputHandler *inputHandler() const;
    QList<Qt3DCore::QNodeId> proxies() const;

    void run() final;
private:
    Q_DECLARE_PRIVATE(LoadProxyDeviceJob)

    InputHandler *m_inputHandler;
    QList<Qt3DCore::QNodeId> m_proxies;
};

typedef QSharedPointer<LoadProxyDeviceJob> LoadProxyDeviceJobPtr;

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_LOADPROXYDEVICEJOB_P_H
