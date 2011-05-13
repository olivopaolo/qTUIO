/*
 * qTUIO - TUIO Interface for Qt
 *
 * src/Overlay.h ---
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

#ifndef OVERLAY_H
#define OVERLAY_H

#include <QApplication>
#include <QWidget>
#include <QEvent>
#include <QTouchEvent>

/** An Overlay widget shows all the QTouchPoints received from its
    parent or from any of the parent's descendants. An Overlay widget
    always requires a parent QWidget. */
class Overlay : public QWidget {

  Q_OBJECT ;

  /** Active touch points */
  QMap<QWidget *, QMap<int, QTouchEvent::TouchPoint> > tPts ;

 public:
  Overlay(QWidget *parent) ;
  ~Overlay() ;

 protected:
  void paintEvent(QPaintEvent *event) ;

 private:
  void setTouchPoint(QWidget *widget, const QTouchEvent::TouchPoint &touchPoint) ;
  bool eventFilter(QObject *obj, QEvent *event) ;
} ;

#endif
