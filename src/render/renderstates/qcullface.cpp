/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*!
 * \class QCullFace
 * \brief The QCullFace class specifies whether front or back face culling
 * are enabled
 * \since 5.7
 * \ingroup renderstates
 *
 * QCullFace sets whether the front or back facets are culled.
 * Facets include triangles, quadrilaterals, polygons and rectangles.
 *
 * \sa QFrontFace
 */
#include "qcullface.h"
#include "qcullface_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 *  Constructs a new QCullFace::QCullFace instance with \a parent as parent.
 */
QCullFace::QCullFace(QNode *parent)
    : QRenderState(*new QCullFacePrivate, parent)
{
}

/*! \internal */
QCullFace::~QCullFace()
{
}

/*!
 * \return which culling mode is currently enabled.
 */
QCullFace::CullingMode QCullFace::mode() const
{
    Q_D(const QCullFace);
    return d->m_mode;
}

/*!
 * Sets which faces to cull to \a mode. Default is set to back.
 * \param mode
 */
void QCullFace::setMode(QCullFace::CullingMode mode)
{
    Q_D(QCullFace);
    if (d->m_mode != mode) {
        d->m_mode = mode;
        emit modeChanged(mode);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QCullFace::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QCullFaceData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QCullFace);
    data.mode = d->m_mode;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
