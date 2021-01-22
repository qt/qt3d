/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_QLEVELOFDETAILSWITCH_H
#define QT3DRENDER_QLEVELOFDETAILSWITCH_H

#include <Qt3DRender/qlevelofdetail.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLevelOfDetailSwitchPrivate;

class Q_3DRENDERSHARED_EXPORT QLevelOfDetailSwitch : public QLevelOfDetail
{
    Q_OBJECT

public:
    explicit QLevelOfDetailSwitch(Qt3DCore::QNode *parent = nullptr);
    ~QLevelOfDetailSwitch();

protected:
    explicit QLevelOfDetailSwitch(QLevelOfDetailPrivate &dd, Qt3DCore::QNode *parent = nullptr);
    // TODO Unused remove in Qt6
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QLevelOfDetailSwitch)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLEVELOFDETAILSWITCH_H
