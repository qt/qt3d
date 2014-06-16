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

#ifndef QT3D_RENDER_MATERIALMANAGER_H
#define QT3D_RENDER_MATERIALMANAGER_H

#include <QtGlobal>
#include <QUuid>
#include <Qt3DCore/qabstractresourcesmanager.h>
#include <Qt3DRenderer/rendermaterial.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Material;

namespace Render {

typedef QHandle<RenderMaterial, 16> HMaterial;

class MaterialManager : public QAbstractResourcesManager<RenderMaterial, Material*, 16>
{
public:
    MaterialManager();

    inline bool hasRenderMaterial(const QUuid &id) const { return m_materialByEntity.contains(id); }
    inline bool hasRenderMaterial(Material *id) { return contains(id); }
    inline RenderMaterial *getOrCreateRenderMaterial(Material *id) { return getOrCreateResource(id); }
    inline RenderMaterial *renderMaterial(Material *id) { return lookupResource(id); }
    inline void releaseRenderMaterial(Material *id) { releaseResource(id); }

    RenderMaterial *renderMaterial(const QUuid &id);
    void linkMaterialToEntity(const QUuid &id, HMaterial material);
    HMaterial lookupHandle(const QUuid &id);

private:
    QHash<QUuid, HMaterial> m_materialByEntity;

};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_MATERIALMANAGER_H
