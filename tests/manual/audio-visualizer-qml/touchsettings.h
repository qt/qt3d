// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TOUCHSETTINGS_H
#define TOUCHSETTINGS_H

#include <QtCore/QObject>

class TouchSettings : public QObject
{
    Q_OBJECT
public:
    explicit TouchSettings(QObject *parent = 0);

    Q_INVOKABLE bool isHoverEnabled() const;

};

#endif // TOUCHSETTINGS_H
