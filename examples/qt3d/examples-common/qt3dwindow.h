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

#ifndef QT3DWINDOW_H
#define QT3DWINDOW_H

#include <QWindow>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAspectEngine;
class QAbstractAspect;
class QEntity;
}

namespace Qt3DRender {
class QCamera;
class QFrameGraph;
class QRenderAspect;
}

namespace Qt3DInput {
class QInputAspect;
}

namespace Qt3DLogic {
class QLogicAspect;
}

class Qt3DWindow : public QWindow
{
    Q_OBJECT
public:
    Qt3DWindow(QScreen *screen = nullptr);
    Qt3DWindow(QWindow *parent);
    ~Qt3DWindow();

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setRootEntity(Qt3DCore::QEntity *root);
    void setFrameGraph(Qt3DRender::QFrameGraph *frameGraph);

    Qt3DRender::QCamera *camera() const;

public Q_SLOTS:

Q_SIGNALS:

protected:
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private:
    QScopedPointer<Qt3DCore::QAspectEngine> m_aspectEngine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    // Renderer configuration
    Qt3DRender::QFrameGraph *m_frameGraph;
    Qt3DRender::QCamera *m_defaultCamera;

    // Input configuration

    // Logic configuration

    // Scene
    Qt3DCore::QEntity *m_root;
    Qt3DCore::QEntity *m_userRoot;

    bool m_initialized;
};

QT_END_NAMESPACE

#endif // QT3DWINDOW_H
