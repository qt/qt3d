# Generated from extras.pro.

if(QT_CONFIG___contains___private_tests)
    add_subdirectory(qcuboidgeometry)
    add_subdirectory(qtorusgeometry)
    add_subdirectory(qforwardrenderer)
    add_subdirectory(qfirstpersoncameracontroller)
    add_subdirectory(qorbitcameracontroller)
endif()
if(TARGET Qt::Quick)
    add_subdirectory(qtext2dentity)
endif()
