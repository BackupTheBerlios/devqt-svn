/****************************************************************************
 * Sphere - A Multiplataform IDE for C++                                    *
 * Copyright (C) 2006  Thiago dos Santos Alves                              *
 *                                                                          *
 * This program is free software; you can redistribute it and/or            *
 * modify it under the terms of the GNU Lesser General Public               *
 * License as published by the Free Software Foundation; either             *
 * version 2.1 of the License, or (at your option) any later version.       *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 * Lesser General Public License for more details.                          *
 *                                                                          *
 * You should have received a copy of the GNU Lesser General Public         *
 * License along with this library; if not, write to the Free Software      *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA *
 *                                                                          *
 * Thiago dos Santos Alves disclaims all copyright interest in              *
 * the program `Sphere' (a Multiplataform IDE for C++) written              *
 * by him self.                                                             *
 *                                                                          *
 * Thiago dos Santos Alves, 13 February 2006                                *
 * thiago.salves@gmail.com                                                  *
 ****************************************************************************/

#include <QApplication>
#include <QFont>
#include <QtDebug>

#include <QList>

#include "editor.h"
#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

//	QFont font;
//	font.setFamily("Courier New");
//	font.setPointSize(10);
//	font.setFixedPitch(true);
//
//	Editor diag;
//	diag.setFont(font);
//	diag.setMultilineEdit(true);
//	//diag.setOverwriteMode(true);
//    diag.show();

	MainWindow mainWin;
	mainWin.show();
    
    return app.exec();
}

