TEMPLATE = app
QMAKE_CXXFLAGS= -std=c++11
LIBS += -lGLU -lGL -lGLEW -lglut -lm

SOURCES += \
    cube.cpp \
    InitShader.cpp \
    light_source.cpp \
    sphere.cpp \
    material.cpp \
    drawable.cpp \
    main.cpp \
    particle.cpp \
    objloader.cpp

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
    mesh.h \
    particle.h \
    objloader.h
