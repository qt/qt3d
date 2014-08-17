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

#ifndef QT3D_RENDER_FRAMEGRAPHNODE_H
#define QT3D_RENDER_FRAMEGRAPHNODE_H

#include <Qt3DCore/qobserverinterface.h>
#include <Qt3DCore/qhandle.h>
#include <qglobal.h>
#include <QVector>
#include <QUuid>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template<typename T, int INDEXBITS>
class QHandle;

namespace Render {

class Renderer;
class FrameGraphNode;

typedef QHandle<FrameGraphNode *, 8> HFrameGraphNode;

class FrameGraphNode : public QObserverInterface
{
public:
    FrameGraphNode();
    virtual ~FrameGraphNode();

    enum FrameGraphNodeType {
        InvalidNodeType = 0,
        CameraSelector,
        LayerFilter,
        RenderPassFilter,
        RenderTarget,   // TODO: Add class
        TechniqueFilter,
        Viewport
    };
    FrameGraphNodeType nodeType() const { return m_nodeType; }


    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void setRenderer(Renderer *renderer);
    void setHandle(HFrameGraphNode handle);
    void setParentHandle(HFrameGraphNode parentHandle);
    void appendChildHandle(HFrameGraphNode childHandle);
    void removeChildHandle(HFrameGraphNode childHandle);

    HFrameGraphNode handle() const;
    HFrameGraphNode parentHandle() const;
    QList<HFrameGraphNode> childrenHandles() const;

    FrameGraphNode *parent() const;
    QList<FrameGraphNode *> children() const;
    QUuid frontendUuid() const { return m_frontendUuid; }

protected:
    FrameGraphNode(FrameGraphNodeType nodeType);
    Renderer *m_renderer;
    QUuid m_frontendUuid;

private:
    FrameGraphNodeType m_nodeType;
    bool m_enabled;
    HFrameGraphNode m_handle;
    HFrameGraphNode m_parentHandle;
    QList<HFrameGraphNode> m_childrenHandles;

    friend class FrameGraphVisitor;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_FRAMEGRAPHNODE_H
