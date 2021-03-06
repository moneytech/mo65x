QT       += core gui widgets testlib

TEMPLATE = app
CONFIG += c++17
CONFIG += strict_c++
CONFIG += sdk_no_version_check
QMAKE_CXXFLAGS += -Wno-padded

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addressrange.cpp \
    assembler.cpp \
    assemblerwidget.cpp \
    assemblyresult.cpp \
    bytespinbox.cpp \
    centralwidget.cpp \
    config.cpp \
    cpu.cpp \
    cpustate.cpp \
    cpuwidget.cpp \
    disassembler.cpp \
    disassemblerview.cpp \
    disassemblerwidget.cpp \
    screenwidget.cpp \
    emulator.cpp \
    executionstatistics.cpp \
    filedatastorage.cpp \
    main.cpp \
    mainwindow.cpp \
    memory.cpp \
    memorywidget.cpp \
    mnemonics.cpp \
    runlevel.cpp \
    symboltable.cpp \
    videowidget.cpp \
    wordspinbox.cpp \
    test/assemblertest.cpp \
    test/instructionstest.cpp \
    test/flagstest.cpp

HEADERS += \
    addressrange.h \
    assembler.h \
    assemblerwidget.h \
    assemblyresult.h \
    centralwidget.h \
    commondefs.h \
    commonformatters.h \
    config.h \
    controlcommand.h \
    cpudefs.h \
    cpuinfo.h \
    cpustate.h \
    cpuwidget.h \
    decodetable.h \
    bytespinbox.h \
    cpu.h \
    disassembler.h \
    disassemblerview.h \
    disassemblerwidget.h \
    operandvalue.h \
    screenwidget.h \
    emulator.h \
    emulatorstate.h \
    executionstatistics.h \
    filedatastorage.h \
    instruction.h \
    instructiontable.h \
    instructiontype.h \
    mainwindow.h \
    memory.h \
    memorywidget.h \
    mnemonics.h \
    operandptr.h \
    operandsformat.h \
    processorstatus.h \
    registers.h \
    runlevel.h \
    stackpointer.h \
    symboltable.h \
    uitools.h \
    videowidget.h \
    wordspinbox.h \
    test/assemblertest.h \
    test/instructionstest.h \
    test/flagstest.h

FORMS += \
    assemblerwidget.ui \
    centralwidget.ui \
    cpuwidget.ui \
    disassemblerview.ui \
    disassemblerwidget.ui \
    mainwindow.ui \
    memorywidget.ui \
    videowidget.ui

RESOURCES += resources.qrc

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

test {
  TARGET = $${TARGET}_tests

  SOURCES -= main.cpp
  SOURCES += test/main.cpp
}

message($${TARGET})
