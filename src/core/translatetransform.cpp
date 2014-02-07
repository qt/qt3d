/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "translatetransform.h"

namespace Qt3D {

TranslateTransform::TranslateTransform(Node *parent)
    : AbstractTransform(parent)
{
}

float TranslateTransform::dx() const
{
    return m_translation.x();
}

float TranslateTransform::dy() const
{
    return m_translation.y();
}

float TranslateTransform::dz() const
{
    return m_translation.z();
}

QVector3D TranslateTransform::translation() const
{
    return m_translation;
}

QMatrix4x4 TranslateTransform::matrix() const
{
    QMatrix4x4 r;
    r.translate(m_translation);
    return r;
}

void TranslateTransform::setDx(float arg)
{
    if (arg != m_translation.x()) {
        m_translation.setX(arg);
        emit translateChanged();
    }
}

void TranslateTransform::setDy(float arg)
{
    if (arg != m_translation.y()) {
        m_translation.setY(arg);
        emit translateChanged();
    }
}

void TranslateTransform::setDz(float arg)
{
    if (arg != m_translation.z()) {
        m_translation.setZ(arg);
        emit translateChanged();
    }
}

void TranslateTransform::setTranslation(QVector3D arg)
{
    if (m_translation != arg) {
        m_translation = arg;
        emit translateChanged();
    }
}

} // namespace Qt3D
