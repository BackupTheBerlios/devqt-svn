######################################################################
# Automatically generated by qmake (1.07a) Sat Aug 27 12:18:37 2005
######################################################################

TEMPLATE 		 = app

################################################
# Project files definition
################################################
FORMS			  = mainwindow.ui

SOURCES 		 += main.cpp \
					editor.cpp \
					editorstate.cpp\
					editorwidget.cpp \
					normalstate.cpp \
					persistentstate.cpp \
					multilinestate.cpp \
					mainwindow.cpp \
					multilinepersistentstate.cpp
					

HEADERS 		 += editor.h \
					editorstate.h \
					editorwidget.h \
					normalstate.h \
					persistentstate.h \
					multilinestate.h \
					mainwindow.h \
					multilinepersistentstate.h

RESOURCES		  = sphere.qrc

################################################
# Directory definitions
################################################
DEPENDPATH		+= src forms temp images
UI_DIR			 = forms
UI_HEADERS_DIR	 = temp
UI_SOURCES_DIR	 = temp
MOC_DIR			 = temp
OBJECTS_DIR		 = objs
RCC_DIR			 = temp
DESTDIR 		 = build

################################################
# Project configuration
################################################
TARGET 			 = Sphere

CONFIG 			+= warn_on debug

win32:debug {
	CONFIG += console
}