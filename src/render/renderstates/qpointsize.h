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
#ifndef QT3DRENDER_QPOINTSIZE_H
#define QT3DRENDER_QPOINTSIZE_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPointSizePrivate;

class QT3DRENDERSHARED_EXPORT QPointSize : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(Specification specification READ specification WRITE setSpecification NOTIFY specificationChanged)
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)

public:
    enum Specification {
        StaticValue = 0,
        Programmable = 1
    };
    Q_ENUM(Specification)

    explicit QPointSize(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QPointSize();

    Specification specification() const;
    float value() const;
    bool isProgrammable() const;

public Q_SLOTS:
    void setSpecification(Specification spec);
    void setValue(float value);

Q_SIGNALS:
    void specificationChanged(Specification spec);
    void valueChanged(float value);

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_FINAL;

private:
    Q_DECLARE_PRIVATE(QPointSize)
    QT3D_CLONEABLE(QPointSize)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPOINTSIZE_H
