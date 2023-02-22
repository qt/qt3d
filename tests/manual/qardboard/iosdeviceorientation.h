// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef IOS_DEVICEORIENTATION_H
#define IOS_DEVICEORIENTATION_H

#include <abstractdeviceorientation.h>

class iOSDeviceOrientation : public AbstractDeviceOrientation
{
    Q_OBJECT
public:
    iOSDeviceOrientation(QObject* parent = 0);

public slots:
    bool start() override;
    void stop() override;
};


#endif // IOS_DEVICEORIENTATION_H
