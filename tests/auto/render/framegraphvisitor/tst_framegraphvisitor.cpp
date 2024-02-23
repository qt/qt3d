// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>

#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/framegraphvisitor_p.h>

#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qviewport.h>

#include "testaspect.h"

namespace {



} // anonymous

using FGIdType = QPair<Qt3DCore::QNodeId, Qt3DRender::Render::FrameGraphNode::FrameGraphNodeType>;
using BranchIdsAndTypes = QList<FGIdType>;

Q_DECLARE_METATYPE(QList<BranchIdsAndTypes>)

class tst_FrameGraphVisitor : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void visitFGTree_data()
    {
        QTest::addColumn<Qt3DCore::QEntity *>("rootEntity");
        QTest::addColumn<Qt3DRender::QFrameGraphNode *>("fgRoot");
        QTest::addColumn<QList<BranchIdsAndTypes>>("fgNodeIdsPerBranch");

        {
            Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
            Qt3DRender::QTechniqueFilter *techniqueFilter = new Qt3DRender::QTechniqueFilter(entity);

            QTest::newRow("singleNode") << entity
                                        << static_cast<Qt3DRender::QFrameGraphNode *>(techniqueFilter)
                                        << (QList<BranchIdsAndTypes>()
                                            << (BranchIdsAndTypes() << FGIdType(techniqueFilter->id(), Qt3DRender::Render::FrameGraphNode::TechniqueFilter))
                                            );
        }

        {
            Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
            Qt3DRender::QTechniqueFilter *techniqueFilter = new Qt3DRender::QTechniqueFilter(entity);
            Qt3DRender::QFrustumCulling *frustumCulling = new Qt3DRender::QFrustumCulling(techniqueFilter);
            Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(frustumCulling);

            QTest::newRow("singleBranch") << entity
                                        << static_cast<Qt3DRender::QFrameGraphNode *>(techniqueFilter)
                                        << (QList<BranchIdsAndTypes>()
                                            << (BranchIdsAndTypes()
                                                << FGIdType(noDraw->id(), Qt3DRender::Render::FrameGraphNode::NoDraw)
                                                << FGIdType(frustumCulling->id(), Qt3DRender::Render::FrameGraphNode::FrustumCulling)
                                                << FGIdType(techniqueFilter->id(), Qt3DRender::Render::FrameGraphNode::TechniqueFilter)
                                                )
                                            );
        }
        {
            Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
            Qt3DRender::QTechniqueFilter *techniqueFilter = new Qt3DRender::QTechniqueFilter(entity);
            Qt3DRender::QFrustumCulling *frustumCulling = new Qt3DRender::QFrustumCulling(techniqueFilter);
            Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(frustumCulling);
            Qt3DRender::QViewport *viewPort = new Qt3DRender::QViewport(frustumCulling);

            QTest::newRow("dualBranch") << entity
                                          << static_cast<Qt3DRender::QFrameGraphNode *>(techniqueFilter)
                                          << (QList<BranchIdsAndTypes>()
                                              << (BranchIdsAndTypes()
                                                  << FGIdType(noDraw->id(), Qt3DRender::Render::FrameGraphNode::NoDraw)
                                                  << FGIdType(frustumCulling->id(), Qt3DRender::Render::FrameGraphNode::FrustumCulling)
                                                  << FGIdType(techniqueFilter->id(), Qt3DRender::Render::FrameGraphNode::TechniqueFilter)
                                                  )
                                              << (BranchIdsAndTypes()
                                                  << FGIdType(viewPort->id(), Qt3DRender::Render::FrameGraphNode::Viewport)
                                                  << FGIdType(frustumCulling->id(), Qt3DRender::Render::FrameGraphNode::FrustumCulling)
                                                  << FGIdType(techniqueFilter->id(), Qt3DRender::Render::FrameGraphNode::TechniqueFilter)
                                                  )
                                              );
        }

    }

    void visitFGTree()
    {
        // GIVEN
        QFETCH(Qt3DCore::QEntity *, rootEntity);
        QFETCH(Qt3DRender::QFrameGraphNode *, fgRoot);
        QFETCH(QList<BranchIdsAndTypes>, fgNodeIdsPerBranch);
        QScopedPointer<Qt3DRender::TestAspect> aspect(new Qt3DRender::TestAspect(rootEntity));

        // THEN
        Qt3DRender::Render::FrameGraphManager *fgManager = aspect->nodeManagers()->frameGraphManager();
        Qt3DRender::Render::FrameGraphNode *backendFGRoot = fgManager->lookupNode(fgRoot->id());
        QVERIFY(backendFGRoot != nullptr);


        // WHEN
        Qt3DRender::Render::FrameGraphVisitor visitor(fgManager);
        const std::vector<Qt3DRender::Render::FrameGraphNode *> fgNodes = visitor.traverse(backendFGRoot);

        // THEN
        QCOMPARE(size_t(fgNodeIdsPerBranch.size()), fgNodes.size());

        for (int i = 0; i < fgNodeIdsPerBranch.size(); ++i) {
            const BranchIdsAndTypes brandIdsAndTypes = fgNodeIdsPerBranch.at(i);

            Qt3DRender::Render::FrameGraphNode *fgNode = fgNodes.at(i);
            for (int j = 0; j < brandIdsAndTypes.size(); ++j) {
                const FGIdType idAndType = brandIdsAndTypes.at(j);
                QVERIFY(fgNode != nullptr);
                QCOMPARE(fgNode->peerId(), idAndType.first);
                QCOMPARE(fgNode->nodeType(), idAndType.second);
                fgNode = fgNode->parent();
            }

            QVERIFY(fgNode == nullptr);
        }

        delete rootEntity;
    }
};

QTEST_MAIN(tst_FrameGraphVisitor)

#include "tst_framegraphvisitor.moc"
