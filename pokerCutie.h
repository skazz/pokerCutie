#include "client.h"
//#include "seat.h"
#include <QThread>
#include <QtGui>

class pokerCutie : QMainWindow {
   Q_OBJECT
   QThread workerThread;
public:
   pokerCutie();

   ~pokerCutie();

public slots:
   void slotRequestAction();

   void slotNextRound();

   void onFoldButton();
   void onCallButton();
   void onRaiseButton();

   void onReadyButton();

   void onConnect();

   void onRaise(int n, int a);
   void onFold(int n);
   void onCheck(int n);
   void onCall(int n);
   void onAllin(int n);

   void onUpdateTable();

signals:
   void signalFold();
   void signalCall();
   void signalRaise(int n);

   void signalConnect(const char * ip, const char * port);

   void signalReady();

protected:
   void paintEvent(QPaintEvent * event);
   
private:
   void setButtonsDisabled(bool flag);

   int loadCards(const char * filename);

   QPushButton *fold;
   QPushButton *call;
   QPushButton *raise;

   QPushButton *readyButton;

   QPushButton *connectButton;

   QPixmap *cards;
   QPixmap *back;
   QPainter painter;

   struct {
      int x;
      int y;
      int name;
      int chips;
   } loc[7];

   client *me;
   seat *mySeat;
   seat *player;
   int playerCount;

   bool tableReady;

};
