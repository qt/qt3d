/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DINPUT_QINPUTSETTINGS_H
#define QT3DINPUT_QINPUTSETTINGS_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DInput/qt3dinput_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputSettingsPrivate;

class QT3DINPUTSHARED_EXPORT QInputSettings : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QObject *eventSource READ eventSource WRITE setEventSource NOTIFY eventSourceChanged)
public:
    explicit QInputSettings(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QInputSettings();

    QObject *eventSource() const;

public Q_SLOTS:
    void setEventSource(QObject *eventSource);

Q_SIGNALS:
    void eventSourceChanged(QObject *);

private:
    Q_DECLARE_PRIVATE(QInputSettings)
    QT3D_CLONEABLE(QInputSettings)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QINPUTSETTINGS_H
