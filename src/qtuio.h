/*
    qTUIO - TUIO Interface for Qt

    Original Version by Martin Blankenburg <martin.blankenburg@imis.uni-luebeck.de>
    Integrated into qTUIO by x29a <0.x29a.0@gmail.com>
    Some modifications by Paolo Olivo <olivopao@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QTUIO_H
#define QTUIO_H

#include "qtuio_dll.h"

#include <QThread>
#include <QMap>
#include <QEvent>
#include <QTouchEvent>
#include <QGenericMatrix>
#include "./3rdparty/tuio/TuioListener.h"

namespace TUIO {
    class TuioClient;
    class TuioCursor;
}

class QGraphicsView;
class QGraphicsScene;
class QMainWindow;
class QRect;

class QTuio : public TUIO::TuioListener, public QThread {
public:
    explicit QTuio(QObject *parent = 0);
    ~QTuio();

    void run();

    virtual void addTuioObject(TUIO::TuioObject *tobj);
    virtual void updateTuioObject(TUIO::TuioObject *tobj);
    virtual void removeTuioObject(TUIO::TuioObject *tobj);
    virtual void addTuioCursor(TUIO::TuioCursor *tcur);
    virtual void updateTuioCursor(TUIO::TuioCursor *tcur);
    virtual void removeTuioCursor(TUIO::TuioCursor *tcur);
    virtual void refresh(TUIO::TuioTime ftime);

private:
    struct Tuio2Qt {
        QTouchEvent::TouchPoint tPoint ;
        QWidget *widget ;
    } ;
    bool running;
    int id ; 
    QGraphicsScene *theScene;
    QGraphicsView *theView;
    QWidget *theWidget;
    /** Calibration matrix to convert TUIO [0,1] coordinates to screen
	coordinates. */
    QMatrix3x3 *calibration;
    QRect screenRect;
    TUIO::TuioClient *tuioClient;
    QMap<int, QTouchEvent::TouchPoint> *qTouchPointMap;
    QMap<long, Tuio2Qt> tuio2Qt ;
    QMap<QWidget *, QList<long> > widgets ;
    bool tuioToQt(TUIO::TuioCursor *tcur, QEvent::Type eventType);
    /** Update the TouchPoint correspondent to the argument
      TuioCursor.  This methos exclusively updates the position
      variables. */
    void updateTouch(TUIO::TuioCursor *tcur) ;
    QPoint norm2Screen(const QPointF &norm) const ;
    QPointF mapWidgetToGlobal(QWidget *widget, const QPointF &pos) const ;
} ;

#endif // QTUIO_H
