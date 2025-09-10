// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCOMPUTECOMMAND_H
#define QT3DRENDER_QCOMPUTECOMMAND_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QComputeCommandPrivate;

class Q_3DRENDERSHARED_EXPORT QComputeCommand : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(int workGroupX READ workGroupX WRITE setWorkGroupX NOTIFY workGroupXChanged)
    Q_PROPERTY(int workGroupY READ workGroupY WRITE setWorkGroupY NOTIFY workGroupYChanged)
    Q_PROPERTY(int workGroupZ READ workGroupZ WRITE setWorkGroupZ NOTIFY workGroupZChanged)
    Q_PROPERTY(RunType runType READ runType WRITE setRunType NOTIFY runTypeChanged REVISION(2, 13))

public:
    enum RunType {
        Continuous = 0,
        Manual
    };
    Q_ENUM(RunType)

    explicit QComputeCommand(Qt3DCore::QNode *parent = nullptr);
    ~QComputeCommand();

    int workGroupX() const;
    int workGroupY() const;
    int workGroupZ() const;
    RunType runType() const;

public Q_SLOTS:
    void setWorkGroupX(int workGroupX);
    void setWorkGroupY(int workGroupY);
    void setWorkGroupZ(int workGroupZ);
    Q_REVISION(2, 13) void setRunType(RunType runType);

    Q_REVISION(2, 13) void trigger(int frameCount = 1);
    Q_REVISION(2, 13) void trigger(int workGroupX, int workGroupY, int workGroupZ, int frameCount = 1);

Q_SIGNALS:
    void workGroupXChanged();
    void workGroupYChanged();
    void workGroupZChanged();
    void runTypeChanged();

private:
    Q_DECLARE_PRIVATE(QComputeCommand)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCOMPUTECOMMAND_H
