TEMPLATE = app
QMAKE_CXXFLAGS= -std=c++11
LIBS += -lGLU -lGL -lGLEW -lglut

SOURCES += \
    cube.cpp \
    InitShader.cpp \
    light_source.cpp \
    sphere.cpp \
    material.cpp \
    drawable.cpp \
    lab7.cpp

OTHER_FILES += \
    Makefile \
    vshader.glsl \
    fshader.glsl

HEADERS += \
    cube.h \
    light_source.h \
    material.h \
    sphere.h \
    drawable.h \
    include/Angel.h \
    include/CheckError.h \
    include/mat.h \
    include/vec.h \
    mesh.h
