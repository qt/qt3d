INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/../btBulletDynamicsCommon.h \ \
    $$PWD/../btBulletCollisionCommon.h \
    $$PWD/ConstraintSolver/btConeTwistConstraint.h \
    $$PWD/ConstraintSolver/btConstraintSolver.h \
    $$PWD/ConstraintSolver/btContactConstraint.h \
    $$PWD/ConstraintSolver/btContactSolverInfo.h \
    $$PWD/ConstraintSolver/btFixedConstraint.h \
    $$PWD/ConstraintSolver/btGearConstraint.h \
    $$PWD/ConstraintSolver/btGeneric6DofConstraint.h \
    $$PWD/ConstraintSolver/btGeneric6DofSpringConstraint.h \
    $$PWD/ConstraintSolver/btGeneric6DofSpring2Constraint.h \
    $$PWD/ConstraintSolver/btHinge2Constraint.h \
    $$PWD/ConstraintSolver/btHingeConstraint.h \
    $$PWD/ConstraintSolver/btJacobianEntry.h \
    $$PWD/ConstraintSolver/btPoint2PointConstraint.h \
    $$PWD/ConstraintSolver/btSequentialImpulseConstraintSolver.h \
    $$PWD/ConstraintSolver/btNNCGConstraintSolver.h \
    $$PWD/ConstraintSolver/btSliderConstraint.h \
    $$PWD/ConstraintSolver/btSolve2LinearConstraint.h \
    $$PWD/ConstraintSolver/btSolverBody.h \
    $$PWD/ConstraintSolver/btSolverConstraint.h \
    $$PWD/ConstraintSolver/btTypedConstraint.h \
    $$PWD/ConstraintSolver/btUniversalConstraint.h \
    $$PWD/Dynamics/btActionInterface.h \
    $$PWD/Dynamics/btDiscreteDynamicsWorld.h \
    $$PWD/Dynamics/btDiscreteDynamicsWorldMt.h \
    $$PWD/Dynamics/btSimulationIslandManagerMt.h \
    $$PWD/Dynamics/btDynamicsWorld.h \
    $$PWD/Dynamics/btSimpleDynamicsWorld.h \
    $$PWD/Dynamics/btRigidBody.h \
    $$PWD/Vehicle/btRaycastVehicle.h \
    $$PWD/Vehicle/btVehicleRaycaster.h \
    $$PWD/Vehicle/btWheelInfo.h \
    $$PWD/Featherstone/btMultiBody.h \
    $$PWD/Featherstone/btMultiBodyConstraintSolver.h \
    $$PWD/Featherstone/btMultiBodyDynamicsWorld.h \
    $$PWD/Featherstone/btMultiBodyLink.h \
    $$PWD/Featherstone/btMultiBodyLinkCollider.h \
    $$PWD/Featherstone/btMultiBodySolverConstraint.h \
    $$PWD/Featherstone/btMultiBodyConstraint.h \
    $$PWD/Featherstone/btMultiBodyJointLimitConstraint.h \
    $$PWD/Featherstone/btMultiBodyConstraint.h \
    $$PWD/Featherstone/btMultiBodyPoint2Point.h \
    $$PWD/Featherstone/btMultiBodyFixedConstraint.h \
    $$PWD/Featherstone/btMultiBodySliderConstraint.h \
    $$PWD/Featherstone/btMultiBodyJointMotor.h \
    $$PWD/MLCPSolvers/btDantzigLCP.h \
    $$PWD/MLCPSolvers/btDantzigSolver.h \
    $$PWD/MLCPSolvers/btMLCPSolver.h \
    $$PWD/MLCPSolvers/btMLCPSolverInterface.h \
    $$PWD/MLCPSolvers/btPATHSolver.h \
    $$PWD/MLCPSolvers/btSolveProjectedGaussSeidel.h \
    $$PWD/MLCPSolvers/btLemkeSolver.h \
    $$PWD/MLCPSolvers/btLemkeAlgorithm.h \
    $$PWD/Character/btCharacterControllerInterface.h \
    $$PWD/Character/btKinematicCharacterController.h

SOURCES += \
    $$PWD/Character/btKinematicCharacterController.cpp \
    $$PWD/ConstraintSolver/btConeTwistConstraint.cpp \
    $$PWD/ConstraintSolver/btContactConstraint.cpp \
    $$PWD/ConstraintSolver/btFixedConstraint.cpp \
    $$PWD/ConstraintSolver/btGearConstraint.cpp \
    $$PWD/ConstraintSolver/btGeneric6DofConstraint.cpp \
    $$PWD/ConstraintSolver/btGeneric6DofSpringConstraint.cpp \
    $$PWD/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp \
    $$PWD/ConstraintSolver/btHinge2Constraint.cpp \
    $$PWD/ConstraintSolver/btHingeConstraint.cpp \
    $$PWD/ConstraintSolver/btPoint2PointConstraint.cpp \
    $$PWD/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
    $$PWD/ConstraintSolver/btNNCGConstraintSolver.cpp \
    $$PWD/ConstraintSolver/btSliderConstraint.cpp \
    $$PWD/ConstraintSolver/btSolve2LinearConstraint.cpp \
    $$PWD/ConstraintSolver/btTypedConstraint.cpp \
    $$PWD/ConstraintSolver/btUniversalConstraint.cpp \
    $$PWD/Dynamics/btDiscreteDynamicsWorld.cpp \
    $$PWD/Dynamics/btDiscreteDynamicsWorldMt.cpp \
    $$PWD/Dynamics/btSimulationIslandManagerMt.cpp \
    $$PWD/Dynamics/btRigidBody.cpp \
    $$PWD/Dynamics/btSimpleDynamicsWorld.cpp \
    $$PWD/Vehicle/btRaycastVehicle.cpp \
    $$PWD/Vehicle/btWheelInfo.cpp \
    $$PWD/Featherstone/btMultiBody.cpp \
    $$PWD/Featherstone/btMultiBodyConstraintSolver.cpp \
    $$PWD/Featherstone/btMultiBodyDynamicsWorld.cpp \
    $$PWD/Featherstone/btMultiBodyJointLimitConstraint.cpp \
    $$PWD/Featherstone/btMultiBodyConstraint.cpp \
    $$PWD/Featherstone/btMultiBodyPoint2Point.cpp \
    $$PWD/Featherstone/btMultiBodyFixedConstraint.cpp \
    $$PWD/Featherstone/btMultiBodySliderConstraint.cpp \
    $$PWD/Featherstone/btMultiBodyJointMotor.cpp \
    $$PWD/MLCPSolvers/btDantzigLCP.cpp \
    $$PWD/MLCPSolvers/btMLCPSolver.cpp \
    $$PWD/MLCPSolvers/btLemkeAlgorithm.cpp

