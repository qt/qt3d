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

#ifndef QT3D_QRENDERTARGET_H
#define QT3D_QRENDERTARGET_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QRenderTargetPrivate;
class QRenderAttachment;

class QT3DRENDERERSHARED_EXPORT QRenderTarget : public QComponent
{
    Q_OBJECT
public:
    explicit QRenderTarget(QNode *parent = 0);

    void copy(const QNode *ref) Q_DECL_OVERRIDE;

    void addAttachment(QRenderAttachment *attachment);
    void removeAttachment(QRenderAttachment *attachment);
    QList<QRenderAttachment *> attachments() const;

protected:
    Q_DECLARE_PRIVATE(QRenderTarget)
    QRenderTarget(QRenderTargetPrivate &dd, QNode *parent = 0);
    QRenderTarget *doClone(bool isClone = true) const Q_DECL_OVERRIDE;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QRENDERTARGET_H
