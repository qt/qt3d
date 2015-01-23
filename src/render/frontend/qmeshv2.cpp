/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qmeshv2.h"

#include <private/qgeometryrenderer_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QMeshV2Private : public QGeometryRendererPrivate
{
public:
    Q_DECLARE_PUBLIC(QMeshV2)
    QMeshV2Private();
    QUrl m_source;
};

QMeshV2Private::QMeshV2Private()
    : QGeometryRendererPrivate()
{
}

QMeshV2::QMeshV2(QNode *parent)
    : QGeometryRenderer(*new QMeshV2Private(), parent)
{
}

QMeshV2::~QMeshV2()
{
    QGeometryRenderer::cleanup();
}

QUrl QMeshV2::source() const
{
    Q_D(const QMeshV2);
    return d->m_source;
}

void QMeshV2::setSource(const QUrl &source)
{
    Q_D(QMeshV2);
    if (d->m_source == source)
        return;

    d->m_source = source;
    emit sourceChanged(source);
}


void QMeshV2::copy(const QNode *ref)
{
    QGeometryRenderer::copy(ref);
    const QMeshV2 *mesh = static_cast<const QMeshV2 *>(ref);
    d_func()->m_source = mesh->d_func()->m_source;
}

}

QT_END_NAMESPACE
