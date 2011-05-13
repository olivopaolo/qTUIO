/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QApplication>

#include "mainwindow.h"
#include "qtuio.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv) ;
  MainWindow window ;
  window.show() ;
  //window.showMaximized();
  window.showFullScreen() ;
  QTuio qTUIO(&window) ;
  QStringList args = app.arguments() ;
  if (args.size() > 1) {
    if (args[1] == "-h" || args[1] == "--help") {
      printf("Usage: %s [OPTION]\n\n", args[0].toLocal8Bit().data()) ;
      printf("Mandatory arguments to long options are mandatory for short options too.\n") ;
      printf("  -c, --calib MATRIX   specificy a 3x3 calibration matrix to fit [0,1] TUIO coordinates to screen coordinates\n") ;
      printf("  -h, --help           display this help and exit\n") ;
      exit(0) ;
    } else if (args[1] == "-c" || args[1] == "--calib") {
      if (args.size() < 11) {
	printf("Error! Calibration matrix requires 9 values.\n") ;
	exit(-1) ;
      } else {
	QMatrix3x3 calibration ;
	qreal *data = calibration.data() ;
	for (int i = 0; i < 9; ++i)
	  data[i] = args[i+2].toDouble() ;
	qTUIO.setTuioCalibration(calibration) ;
      }
    }
  }  
  qTUIO.run();
  return app.exec();
}
