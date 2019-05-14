QT -= core
QT -= gui

TARGET = v_repExtShareMemory
TEMPLATE = lib

DEFINES -= UNICODE
DEFINES += QT_COMPIL
CONFIG += shared
CONFIG += c++0x
INCLUDEPATH += "../include"
unix:LIBS += -lrt

*-msvc* {
        QMAKE_CXXFLAGS += -O2
        QMAKE_CXXFLAGS += -W3
}
*-g++* {
        QMAKE_CXXFLAGS += -O3
        QMAKE_CXXFLAGS += -Wall
        QMAKE_CXXFLAGS += -Wno-unused-parameter
        QMAKE_CXXFLAGS += -Wno-strict-aliasing
        QMAKE_CXXFLAGS += -Wno-empty-body
        QMAKE_CXXFLAGS += -Wno-write-strings

        QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
        QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
        QMAKE_CXXFLAGS += -Wno-narrowing

        QMAKE_CFLAGS += -O3
        QMAKE_CFLAGS += -Wall
        QMAKE_CFLAGS += -Wno-strict-aliasing
        QMAKE_CFLAGS += -Wno-unused-parameter
        QMAKE_CFLAGS += -Wno-unused-but-set-variable
        QMAKE_CFLAGS += -Wno-unused-local-typedefs
        QMAKE_CXXFLAGS += -std=c++0x

}

win32 {
    DEFINES += WIN_VREP
}

macx {
    DEFINES += MAC_VREP
}

unix:!macx {
    DEFINES += LIN_VREP
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

HEADERS += \
      access.h\
      v_repExtShareMemory.h \
      ShareMemoryLib/Cevent.h\
      ShareMemoryLib/CeventContainer.h\
      ShareMemoryLib/CshareMemory.h\
      ShareMemoryLib/CshareMemoryContainer.h\
    ../include/luaFunctionData.h \
    ../include/luaFunctionDataItem.h \
    ../include/v_repLib.h

SOURCES += \
      access.cpp\
      v_repExtShareMemory.cpp \
      ShareMemoryLib/Cevent.cpp\
      ShareMemoryLib/CeventContainer.cpp\
      ShareMemoryLib/CshareMemory.cpp\
      ShareMemoryLib/CshareMemoryContainer.cpp\
    ../common/luaFunctionData.cpp \
    ../common/luaFunctionDataItem.cpp \
    ../common/v_repLib.cpp




