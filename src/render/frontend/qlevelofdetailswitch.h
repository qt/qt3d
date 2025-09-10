// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

private:
    Q_DECLARE_PRIVATE(QLevelOfDetailSwitch)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLEVELOFDETAILSWITCH_H
