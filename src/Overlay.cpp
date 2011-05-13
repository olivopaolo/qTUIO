/*
 * qTUIO - TUIO Interface for Qt
 *
 * src/Overlay.cpp ---
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

#include <QPainter>
#include <QDebug>

#include "Overlay.h"
// #define DEBUG

#define CIRCLE_SIZE 30
#define POINT_SIZE 2

Overlay::Overlay(QWidget *parent) : QWidget(parent) {
  setFixedSize(parent->size()) ;
  QApplication::instance()->installEventFilter(this) ;
  setAttribute(Qt::WA_TransparentForMouseEvents);
}

Overlay::~Overlay() {
  QApplication::instance()->removeEventFilter(this) ;
}

void 
Overlay::setTouchPoint(QWidget *widget, const QTouchEvent::TouchPoint &touchPoint) {
  QWidget *parent = parentWidget() ;
  // if the Overlay has a parent
  if (parent->isAncestorOf(widget)) {
    QTouchEvent::TouchPoint newPoint(touchPoint) ;
    QPoint relPos(newPoint.pos().x(), newPoint.pos().y()) ; 
    newPoint.setPos(widget->mapTo(parent, relPos)) ;    
    QMap<int, QTouchEvent::TouchPoint> map = tPts.value(widget) ;
    if (map.contains(newPoint.id())) {
      QPointF oldPos = map.value(newPoint.id()).pos() ;
      QPointF newPos = newPoint.pos() ;
      map.insert(newPoint.id(), newPoint) ;
      tPts.insert(widget, map) ;
      update(QRect(QPoint(oldPos.x(), oldPos.y()), QPoint(newPos.x(), newPos.y())).normalized().adjusted(-CIRCLE_SIZE-2 , -CIRCLE_SIZE-2, +CIRCLE_SIZE+2, +CIRCLE_SIZE+2)) ;
    } else {
      QPointF pos = newPoint.pos() ;
      map.insert(newPoint.id(), newPoint) ;
      tPts.insert(widget, map) ;
      update(QRect(pos.x(), pos.y(), 0, 0).adjusted(-CIRCLE_SIZE-2, -CIRCLE_SIZE-2, +CIRCLE_SIZE+2, +CIRCLE_SIZE+2)) ;

    }
  }
}

void
Overlay::paintEvent(QPaintEvent *) {
  QPainter painter(this) ;
  painter.setPen(QColor(255, 0, 0)) ;
  QMutableMapIterator<QWidget *, QMap<int, QTouchEvent::TouchPoint> > widgets(tPts);
  while (widgets.hasNext()) {
    QMap<int, QTouchEvent::TouchPoint> map = widgets.next().value() ;
    QMutableMapIterator<int, QTouchEvent::TouchPoint> touches(map);
    while (touches.hasNext()) {
      QTouchEvent::TouchPoint touchPoint = touches.next().value();
      if (touchPoint.state() != Qt::TouchPointReleased) {
	 painter.drawEllipse(touchPoint.pos().x() - CIRCLE_SIZE, touchPoint.pos().y() - CIRCLE_SIZE, CIRCLE_SIZE * 2, CIRCLE_SIZE * 2) ;
	 painter.drawEllipse(touchPoint.pos().x() - POINT_SIZE, touchPoint.pos().y() - POINT_SIZE, POINT_SIZE * 2, POINT_SIZE * 2) ;
      } else {
	touches.remove() ;
      }
    }
    if (map.size() == 0) widgets.remove() ;
  }
}

bool
Overlay::eventFilter(QObject *obj, QEvent *event) {
  QWidget *parent = parentWidget() ;
  QWidget *target = qobject_cast<QWidget *>(obj);

  if (target == parent && event->type() == QEvent::Resize) {
    QResizeEvent *rEvent = static_cast<QResizeEvent *>(event) ;
    setFixedSize(rEvent->size()) ;
  }  

  if (target == parent || (target && parent->isAncestorOf(target))) {
#ifdef DEBUG
      QString typ ;
      QTouchEvent *tevent ;
      QList<QTouchEvent::TouchPoint> pts ;
      switch (event->type()) {
      case QEvent::TouchBegin: 
	tevent = static_cast<QTouchEvent *>(event) ;
	typ = "TouchBegin " ;
	pts = tevent->touchPoints() ;
	for(int i = 0; i<pts.size(); i++) {
	  typ += "\n   id: " + QString::number(pts[i].id()) ;
	  typ += " (" + QString::number(pts[i].normalizedPos().x()) + ", " + QString::number(pts[i].normalizedPos().y()) + ")";
	  typ += " state: " + QString::number(pts[i].state()) ;
	  if (pts[i].isPrimary()) typ += " (PRIMARY)"; 
	}
	qDebug() << obj << " " << typ ;
	break ;
      case QEvent::TouchUpdate: 
	tevent = static_cast<QTouchEvent *>(event) ;
	typ = "TouchUpdate " ;
	pts = tevent->touchPoints() ;
	for(int i = 0; i<pts.size(); i++) {
	  typ += "\n   id: " + QString::number(pts[i].id()) ;
	  typ += " (" + QString::number(pts[i].pos().x()) + ", " + QString::number(pts[i].pos().y()) + ")";
	  typ += " state: " + QString::number(pts[i].state()) ;
	  if (pts[i].isPrimary()) typ += " (PRIMARY)"; 
	}
	qDebug() << obj << " " << typ ;
	break ;
      case QEvent::TouchEnd:
	tevent = static_cast<QTouchEvent *>(event) ;
	typ = "TouchEnd " ;
	pts = tevent->touchPoints() ;
	for(int i = 0; i<pts.size(); i++) {
	  typ += "\n   id: " + QString::number(pts[i].id()) ;
	  typ += " (" + QString::number(pts[i].normalizedPos().x()) + ", " + QString::number(pts[i].normalizedPos().y()) + ")";
	  typ += " state: " + QString::number(pts[i].state()) ;
	  if (pts[i].isPrimary()) typ += " (PRIMARY)"; 
	}
	qDebug() << obj << " " << typ ;
	break ;
      default : break ;
      }
#endif
      QTouchEvent *tEvent ;
      QList<QTouchEvent::TouchPoint> newPts ;
      switch (event->type()) {
      case QEvent::TouchBegin: 
      case QEvent::TouchUpdate: 
      case QEvent::TouchEnd:
	tEvent = static_cast<QTouchEvent *>(event) ;
	newPts = tEvent->touchPoints() ;
	for(int i = 0; i<newPts.size(); i++)
	  setTouchPoint(target, newPts[i]) ;
	break ;
      default: break ;
      }
  }
  return QObject::eventFilter(obj, event) ;
}

