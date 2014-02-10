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

#ifndef RENDERTECHNIQUE_H
#define RENDERTECHNIQUE_H

#include <QVector>
#include <QStringList>

namespace Qt3D
{

class Technique;
class RenderShader;
class RenderBin;
class Parameter;
class DrawStateSet;

class RenderTechnique
{
public:
    RenderTechnique(Technique* peer);

    unsigned int passCount() const;

    void setShaderForPass(unsigned int pass, RenderShader* s);
    RenderShader* shaderForPass(unsigned int pass);

    void setBinForPass(unsigned int pas, RenderBin* bin);
    RenderBin* binForPass(unsigned int pass) const;

    DrawStateSet* stateSetForPass(unsigned int pass) const;

    QString glslNameForMeshAttribute(unsigned int pass, QString meshAttributeName);

    QStringList glslNamesForUniformParameter(QString pName) const;

    // FIXME using front-end classes here, not ideal
    Parameter* parameterByName(QString name) const;
private:
    Technique* m_peer;
    unsigned int m_passCount;

    // really need more than just the shader here, i.e other
    // technique state
    QVector<RenderShader*> m_passShader;
    QVector<RenderBin*> m_passBin;




};

} // of namespace Qgt3D

#endif // RENDERTECHNIQUE_H
