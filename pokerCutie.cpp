#include "pokerCutie.h"


pokerCutie::pokerCutie() {
   setWindowTitle("PokerCutie");
   setStyleSheet("background-image: url(./table.png)");
   setGeometry(QRect(20, 20, 800, 600));

   tableFont = QFont(QString("Tahoma"), 12);
   tableFont.setBold(true);
   this->setFont(tableFont);


   tableReady = false;

   loc[0].x = 345;
   loc[0].y = 480;
   loc[0].name = 580;
   loc[0].chips = 465;

   loc[1].x = 131;
   loc[1].y = 480;
   loc[1].name = 580;
   loc[1].chips = 465;

   loc[2].x = 20;
   loc[2].y = 253;
   loc[2].name = 353;
   loc[2].chips = 238;

   loc[3].x = 131;
   loc[3].y = 30;
   loc[3].name = 15;
   loc[3].chips = 135;

   loc[4].x = 354;
   loc[4].y = 30;
   loc[4].name = 15;
   loc[4].chips = 135;

   loc[5].x = 551;
   loc[5].y = 30;
   loc[5].name = 15;
   loc[5].chips = 135;

   loc[6].x = 682;
   loc[6].y = 253;
   loc[6].name = 353;
   loc[6].chips = 238;

   loc[7].x = 551;
   loc[7].y = 480;
   loc[7].name = 580;
   loc[7].chips = 465;

   for(int i = 0; i < 8; i++) {
      time(&(notification[i].start));
      notification[i].text = "";
      notification[i].duration = 2;
   }

   loadCards("./cards.png");
   back = new QPixmap("./back.png");

   me = new client("Cutie");
      
   fold = new QPushButton("Fold", this);
   raise = new QPushButton("Raise", this);
   call = new QPushButton("Check/Call", this);
   fold->setGeometry(QRect(200, 400, 100, 40));
   call->setGeometry(QRect(350, 400, 100, 40));
   raise->setGeometry(QRect(500, 400, 100, 40));
   setButtonsDisabled(true);

   connectButton = new QPushButton("Connect", this);
   connectButton->setGeometry(QRect(350, 150, 100, 40));

   readyButton = new QPushButton("Ready", this);
   readyButton->setGeometry(QRect(350, 150, 100, 40));
   readyButton->setVisible(false);


   connect(fold, SIGNAL(clicked()), this, SLOT(onFoldButton()));
   connect(call, SIGNAL(clicked()), this, SLOT(onCallButton()));
   connect(raise, SIGNAL(clicked()), this, SLOT(onRaiseButton()));

   connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnect()));
   connect(readyButton, SIGNAL(clicked()), this, SLOT(onReadyButton()));

   connect(this, SIGNAL(signalFold()), me, SLOT(fold()));
   connect(this, SIGNAL(signalCall()), me, SLOT(call()));
   connect(this, SIGNAL(signalRaise(int)), me, SLOT(raise(int)));
   connect(this, SIGNAL(signalConnect(const char *, const char *)), me, SLOT(connectToServer(const char *, const char *)));
   connect(this, SIGNAL(signalReady()), me, SLOT(ready()));

   connect(me, SIGNAL(placeBet()), this, SLOT(slotRequestAction()));
   connect(me, SIGNAL(nextRound()), this, SLOT(slotNextRound()));

   connect(me, SIGNAL(updateTable()), this, SLOT(onUpdateTable()));

   connect(me, SIGNAL(playerFolded(int)), this, SLOT(onFold(int)));
   connect(me, SIGNAL(playerCalled(int)), this, SLOT(onCall(int)));
   connect(me, SIGNAL(playerChecked(int)), this, SLOT(onCheck(int)));
   connect(me, SIGNAL(playerRaised(int, int)), this, SLOT(onRaise(int, int)));
   connect(me, SIGNAL(playerAllin(int, int)), this, SLOT(onAllin(int)));

   connect(me, SIGNAL(playerWon(int, int)), this, SLOT(onPlayerWon(int)));


   me->moveToThread(&workerThread);
   workerThread.start();

   show();
}

void pokerCutie::onPlayerWon(int n) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "Winner";

   repaint();
}


void pokerCutie::onFold(int n) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "I Fold";

   repaint();
}

void pokerCutie::onCheck(int n) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "I Check";

   repaint();
}

void pokerCutie::onCall(int n) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "I Call";

   repaint();
}

void pokerCutie::onRaise(int n, int a) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "I Raise";

   repaint();
}

void pokerCutie::onAllin(int n) {
   int seat = (n - me->getSeatNumber() + playerCount) % playerCount;
   time(&(notification[seat].start));
   notification[seat].text = "I'm Allin";

   repaint();
}

void pokerCutie::slotNextRound()
{
   readyButton->setVisible(true);
}

void pokerCutie::onReadyButton()
{
   readyButton->setVisible(false);
   // clear notifications TODO
   emit signalReady();
}

void pokerCutie::onUpdateTable()
{
   tableReady = true;
   repaint();
}

void pokerCutie::paintEvent(QPaintEvent * event)
{
   if(tableReady) {
      int *tmp;
      int tmp2;
      QString name, bet, chips;
      mySeat = me->getMe();
      playerCount = me->getPlayercount();
      player = me->getPlayers();

      painter.begin(this);
      if(mySeat->isInGame()) {
         if(!mySeat->hasFolded()) {
            tmp = mySeat->getHolecards();
            painter.drawPixmap(loc[0].x, loc[0].y, *cards, (tmp[0] / 4)*73+1, (tmp[0] % 4)*98+2, 72, 95);
            painter.drawPixmap(loc[0].x+20, loc[0].y, *cards, (tmp[1] / 4)*73+1, (tmp[1] % 4)*98+2, 72, 95);
         } else {
            painter.drawPixmap(loc[0].x, loc[0].y, *back);
            painter.drawPixmap(loc[0].x+20, loc[0].y, *back);
         }

         name = mySeat->getName();
         bet.setNum(mySeat->getCurrentBet());
         chips.setNum(mySeat->getRemainingChips());
         painter.drawText(loc[0].x, loc[0].name, 92, 10, Qt::AlignCenter, name);
         painter.drawText(loc[0].x, loc[0].chips, 46, 10, Qt::AlignCenter, bet);
         painter.drawText(loc[0].x+46, loc[0].chips, 46, 10, Qt::AlignCenter, chips);
      }

      if(me->onFlop()) {
         tmp = me->getFlop();
         painter.drawPixmap(200, 253, *cards, (tmp[0] / 4)*73+1, (tmp[0] % 4)*98+2, 72, 95);
         painter.drawPixmap(282, 253, *cards, (tmp[1] / 4)*73+1, (tmp[1] % 4)*98+2, 72, 95);
         painter.drawPixmap(364, 253, *cards, (tmp[2] / 4)*73+1, (tmp[2] % 4)*98+2, 72, 95);

         // painter.drawPixmap(118, 253, *back);
      }

      if(me->onTurn()) {
         tmp2 = me->getTurn();
         painter.drawPixmap(446, 253, *cards, (tmp2 / 4)*73+1, (tmp2 % 4)*98+2, 72, 95);
      }

      if(me->onRiver()) {
         tmp2 = me->getRiver();
         painter.drawPixmap(528, 253, *cards, (tmp2 / 4)*73+1, (tmp2 % 4)*98+2, 72, 95);
      }

      // paint other players
      int seat = (me->getSeatNumber() + 1) % playerCount;
      for(int i = 1; i < playerCount; i++) {
         if(player[seat].isInGame()) {
            if(!player[seat].hasFolded()) {
               if(!player[seat].cardsOnTable()) {
                  painter.drawPixmap(loc[i].x, loc[i].y, *back);
                  painter.drawPixmap(loc[i].x+20, loc[i].y, *back);
               } else {
                  tmp = player[seat].getHolecards();
                  painter.drawPixmap(loc[i].x, loc[i].y, *cards, (tmp[0] / 4)*73+1, (tmp[0] % 4)*98+2, 72, 95);
                  painter.drawPixmap(loc[i].x+20, loc[i].y, *cards, (tmp[1] / 4)*73+1, (tmp[1] % 4)*98+2, 72, 95);
               }
            }

            name = player[seat].getName();
            bet.setNum(player[seat].getCurrentBet());
            chips.setNum(player[seat].getRemainingChips());

            painter.drawText(loc[i].x, loc[i].name, 92, 10, Qt::AlignCenter, name);
            painter.drawText(loc[i].x, loc[i].chips, 46, 10, Qt::AlignCenter,  bet);
            painter.drawText(loc[i].x+46, loc[i].chips, 46, 10, Qt::AlignCenter, chips);

         }
         seat = (seat+1) % playerCount;
      }

      // paint notifications
      time_t now;
      time(&now);
      int elapsed;
      for(int i = 0; i < playerCount; i++) {
         elapsed = difftime(now, notification[i].start);
         if(elapsed < notification[i].duration) {
            painter.fillRect(QRect(loc[i].x+5, loc[i].y+25, 82, 40), QColor(150, 150, 150, 220));
            painter.drawText(loc[i].x+5, loc[i].y+25, 82, 40, Qt::AlignCenter, notification[i].text);
         }
      }

      painter.end();
   }
}

void pokerCutie::slotRequestAction() {
   setButtonsDisabled(false);
}

void pokerCutie::onFoldButton() {
   setButtonsDisabled(true);
   emit signalFold();
}

void pokerCutie::onCallButton() {
   setButtonsDisabled(true);
   emit signalCall();
}

void pokerCutie::onRaiseButton() {
   bool ok = false;

   int value = QInputDialog::getInt ( this, QString("Raise"), QString("Raise by:"), me->getMinimumBet(), me->getMinimumBet(), mySeat->getRemainingChips(), 10, &ok);
   
   if(ok) {
      setButtonsDisabled(true);
      emit signalRaise(value);
   }
}

void pokerCutie::onConnect() {
   bool ok = false;
   QString ip = QInputDialog::getText ( this, QString("Connect"), QString("Enter Server Ip"), QLineEdit::Normal, QString("127.0.0.1"), &ok );

   if(ok) {
      connectButton->setVisible(false);
      connectButton->setDisabled(true);

      emit signalConnect(ip.toStdString().c_str(), "3031");
   }
}

void pokerCutie::setButtonsDisabled(bool flag)
{
   fold->setDisabled(flag);
   call->setDisabled(flag);
   raise->setDisabled(flag);
}

int pokerCutie::loadCards(const char * filename)
{
   cards = new QPixmap(filename);
   return 0;
}

pokerCutie::~pokerCutie() {
   workerThread.quit();
   workerThread.wait();
}

int main(int argc, char **argv) {
   QApplication app(argc, argv);
   pokerCutie cutie;

   return app.exec();
   
}
