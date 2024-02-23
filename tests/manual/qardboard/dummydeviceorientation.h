// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef DUMMY_DEVICEORIENTATION_H
#define DUMMY_DEVICEORIENTATION_H

#include "abstractdeviceorientation.h"

class DummyDeviceOrientation : public AbstractDeviceOrientation
{
    Q_OBJECT
public:
    DummyDeviceOrientation(QObject* parent = 0);

public slots:
    bool start() override;
    void stop() override;
};


#endif // DUMMY_DEVICEORIENTATION_H
