/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#include "qrenderstatecreatedchange_p.h"
#include <Qt3DCore/private/qnodecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \internal
 * \class Qt3DRender::QRenderStateCreatedChange
 * \inheaderfile Qt3DRender/QRenderStateCreatedChange
 * \brief The QRenderStateCreatedChange class
 * \since 5.7
 * \inmodule Qt3DRender
 * \ingroup renderstates
 */

/*! \internal */
class QRenderStateCreatedChangeBasePrivate : public Qt3DCore::QNodeCreatedChangeBasePrivate
{
public:
    QRenderStateCreatedChangeBasePrivate(const QRenderState *renderState)
        : Qt3DCore::QNodeCreatedChangeBasePrivate(renderState)
        , m_type(QRenderStatePrivate::get(renderState)->m_type)
    {
    }

    Render::StateMask m_type;
};

/*
 * The constructor creates a new QRenderStateCreatedChangeBase::QRenderStateCreatedChangeBase
 * instance with the specified \a renderState.
 */
QRenderStateCreatedChangeBase::QRenderStateCreatedChangeBase(const QRenderState *renderState)
    : QNodeCreatedChangeBase(*new QRenderStateCreatedChangeBasePrivate(renderState), renderState)
{
}

/*
 * Returns the current render state type.
 */
Render::StateMask QRenderStateCreatedChangeBase::renderStateType() const
{
    Q_D(const QRenderStateCreatedChangeBase);
    return d->m_type;
}

QT_END_NAMESPACE

} // namespace Qt3DRender
