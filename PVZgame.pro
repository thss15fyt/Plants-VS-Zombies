#-------------------------------------------------
#
# Project created by QtCreator 2016-08-28T10:58:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PVZgame
TEMPLATE = app


SOURCES += main.cpp\
        pvz.cpp \
    welcome.cpp \
    help.cpp \
    ingame.cpp \
    previewzombie.cpp \
    plant.cpp \
    card.cpp \
    block.cpp \
    zombie.cpp \
    randnumber.cpp \
    peaball.cpp

HEADERS  += pvz.h \
    welcome.h \
    help.h \
    ingame.h \
    previewzombie.h \
    ZombieSize.h \
    FieldSize.h \
    plant.h \
    src/plants/Peashooter/PlantSize.h \
    PlantSize.h \
    card.h \
    block.h \
    zombie.h \
    randnumber.h \
    WeaponSize.h \
    peaball.h \
    PeaBallSize.h

FORMS    += pvz.ui \
    welcome.ui \
    help.ui \
    ingame.ui

RESOURCES += \
    src.qrc

ICON = icon.icns

QT += multimedia
