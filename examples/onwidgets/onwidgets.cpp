/*
 * qTUIO - TUIO Interface for Qt
 *
 * examples/onwidgets/onwidgets.cpp ---
 *
 * Initial software  
 * Author: Paolo Olivo <olivopaolo@tiscali.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QApplication>
#include <QMainWindow>

#include "qtuio.h"

#include "Overlay.h"
#include "ui_onwidgets.h"

int
main(int argc, char *argv[]) {
  QApplication app(argc, argv) ;
  QMainWindow *window = new QMainWindow ;
  Ui::overlayTest ui ;
  ui.setupUi(window) ;
  foreach (QWidget *widget, QApplication::allWidgets())
    widget->setAttribute(Qt::WA_AcceptTouchEvents) ;
  // --- add Overlay ---
  Overlay *overlay = new Overlay(window) ;  
  // --- Add qTUIO ---
  QTuio qTUIO(window) ;
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
  // ---
  window->show() ;
  window->raise() ;
  return app.exec() ;
}
