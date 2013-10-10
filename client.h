#include "pack.h"
#include "seat.h"

#include <QThread>
#include <QtGui>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>


class client : public QObject{
   Q_OBJECT
   QThread workerThread;
public:
 
   client(const char *name);

   seat *getMe() { return me; };

   int getSeatNumber() { return seatNumber; };

   bool onFlop() { return _onFlop; }

   int *getFlop() { return flop; };

   bool onTurn() { return _onTurn; };

   int getTurn() { return turn; };

   bool onRiver() { return _onRiver; };

   int getRiver() { return river; };

   int getPlayercount() { return playerCount; };

   seat *getPlayers() { return player; };

   int getMinimumBet() { return minimumBet; };

   ~client();

public slots:

   void connectToServer(const char *remoteHost, const char *port);
   // communication to server
   void ready();

   void fold();

   void raise(int amount);

   void call();

   void check();

   void allin();

   void bail();


signals:
   /* 
      communication from server
      gets called when server requests your action
      answer with check() call() raise()...
   */
   void placeBet(); // <- needed!

   void nextRound();

   void updateTable();


   void playerFolded(int n);

   void playerRaised(int n, int amount);

   void playerCalled(int n);

   void playerChecked(int n);

   void playerAllin(int n, int amount);

   void setBlinds(int small, int big);

   void playerWon(int n, int amount);

   void tie();

   void playerEliminated(int n);

   void playerLeft(int n);

   void actionAccepted();

   void actionUnknown();

   void actionNotAllowed();

   void betTooLow();

private:
   void newRound();

   void setHolecards(int c1, int c2);

   void setFlop(int c1, int c2, int c3);

   void setTurn(int c1);

   void setRiver(int c1);
   
   void playerHolecards(int n, int c1, int c2);

   void playerIsDealer(int n);

   void playerIsSmallBlind(int n);

   void playerIsBigBlind(int n);

   void setPlayerName(int n, char *name);

   void setPlayerCount(int n);

   void setSeatNumber(int n);

   void setStartingChips(int amount);


   seat *me;

   int seatNumber;

   const char *name;

   seat player[8];

   int playerCount;

   int minimumBet, smallBlind, bigBlind, toCall;

   int flop[3];

   int turn, river;

   bool _onFlop, _onTurn, _onRiver;

   int sockfd;

   int gameLoop();

   int read(unsigned char *buf);

   int sendAll(int fd, unsigned char *msg, int msg_len);
};
