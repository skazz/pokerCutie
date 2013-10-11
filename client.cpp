#include "client.h"

using namespace std;

client::client(const char *_name) {
   name = _name;
}

client::~client() {
   close(sockfd);
}

void client::connectToServer(const char *remoteHost, const char *port) {
   struct addrinfo hints, *res;

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   getaddrinfo(remoteHost, port, &hints, &res);

   sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   ::connect(sockfd, res->ai_addr, res->ai_addrlen);

   
   unsigned char msg[64];
   int msg_len = pack(msg, "bs", 0, name);
   sendAll(sockfd, msg, msg_len);

   gameLoop();
}

void client::ready() {
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 10);
   sendAll(sockfd, msg, msg_len);

   gameLoop();
}

void client::fold() {
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 1);
   sendAll(sockfd, msg, msg_len);

   // resume gameloop
   gameLoop();
}

void client::raise(int amount) {
   int16_t tmp = amount;
   unsigned char msg[4];
   int msg_len;

   msg_len = pack(msg, "bh", 2, tmp);
   sendAll(sockfd, msg, msg_len);

   // resume gameLoop
   gameLoop();
}

void client::call() {
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 3);
   sendAll(sockfd, msg, msg_len);

   // resume gameLoop
   gameLoop();
}

void client::check() {
   unsigned char msg[2];
   int msg_len;
   
   msg_len = pack(msg, "b", 4);
   sendAll(sockfd, msg, msg_len);

   // resume gameLoop
   gameLoop();
}

void client::allin() {
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 5);
   sendAll(sockfd, msg, msg_len);

   // resume gameLoop
   gameLoop();
}

void client::bail() {
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 6);
   sendAll(sockfd, msg, msg_len);

   // resume gameLoop
   gameLoop();
}

// specs in ./list_of_actions
int client::read(unsigned char *buf) {

   unsigned char *p = buf;
   int8_t a, b, c;
   int16_t d, f;
   char s[64];
   int8_t action = *p++;

   switch(action) {
   case 40:
      emit placeBet();
      return 1; // TODO figure out a better way
   case 19:
      emit nextRound();
      return 1;
   case 0:
      emit actionAccepted();
      return 0;
   case 41:
      unpack(p, "b", &a);
      player[a].folds();
      emit playerFolded(a);
      return 0;
   case 42:
      unpack(p, "bh", &a, &d);
      player[a].bets(toCall - player[a].getCurrentBet() + d);
      toCall = player[a].getCurrentBet();
      minimumBet = d;
      emit playerRaised(a, d);
      return 0;
   case 43:
      unpack(p, "b", &a);
      player[a].bets(toCall - player[a].getCurrentBet());
      emit playerCalled(a);
      return 0;
   case 44:
      unpack(p, "b", &a);
      emit playerChecked(a);
      return 0;
   case 45:
      unpack(p, "bh", &a, &d);
      player[a].allin();
      emit playerAllin(a, d);
      return 0;
   case 50:
      unpack(p, "bbb", &a, &b, &c);
      playerHolecards(a, b, c);
      return 0;
   case 30:
      unpack(p, "bb", &a, &b);
      setHolecards(a, b);
      return 0;
   case 31:
      unpack(p, "bbb", &a, &b, &c);
      setFlop(a, b, c);
      return 0;
   case 32:
      unpack(p, "b", &a);
      setTurn(a);
      return 0;
   case 33:
      unpack(p, "b", &a);
      setRiver(a);
      return 0;
   case 21:
      unpack(p, "b", &a);
      newRound();
      playerIsDealer(a);
      return 0;
   case 22:
      unpack(p, "b", &a);
      playerIsSmallBlind(a);
      return 0;
   case 23:
      unpack(p, "b", &a);
      playerIsBigBlind(a);
      return 0;
   case 20:
      unpack(p, "hh", &d, &f);
      smallBlind = d;
      bigBlind = f;
      emit setBlinds(d, f);
      return 0;
   case 52:
      unpack(p, "bh", &a, &d);
      player[a].wins(d);
      emit playerWon(a, d);
      return 0;
   case 51:
      emit tie();
      return 0;
   case 1:
      emit actionUnknown();
      return 0;
   case 2:
      emit actionNotAllowed();
      return 0;
   case 3:
      emit betTooLow();
      return 0;
   case 11:
      unpack(p, "bs", &a, &s);
      setPlayerName(a, s);
      return 0;
   case 10:
      unpack(p, "b", &a);
      setPlayerCount(a);
      return 0;
   case 12:
      unpack(p, "b", &a);
      setSeatNumber(a);
      return 0;
   case 13:
      unpack(p, "b", &a);
      player[a].setInGame(false);
      emit playerEliminated(a);
      return 0;
   case 14:
      unpack(p, "b", &a);
      player[a].setInGame(false);
      emit playerLeft(a);
      return 0;
   case 15:
      unpack(p, "h", &d);
      setStartingChips(d);
      return 0;
   default:
      return -1;
   }

}

int client::gameLoop() {

   unsigned char buf[64];
   unsigned char *p;
   unsigned char tmp_buf[130];
   int tmp_offset = 0;
   int buf_len = sizeof buf;
   int bytes_received = 0;
   int8_t size;

   int actionRequested = 0;

   while((bytes_received = recv(sockfd, buf, buf_len, 0)) > 0) {
      
      memcpy(tmp_buf + tmp_offset, buf, bytes_received);
      tmp_offset += bytes_received;

      if(bytes_received < 1)
         continue;

      size = *tmp_buf;

      while(tmp_offset >= size + 1) {

         p = tmp_buf + 1;
         actionRequested = read(p);

         tmp_offset -= (size + 1);

         memmove(tmp_buf, tmp_buf + size + 1, tmp_offset);

         if(tmp_offset > 0)
            size = *tmp_buf;

         // stop gameLoop to wait for action
         // ugly but safe
         // after request server doesnt send anything until client sent his action
         // call gameloop when action is sent
         if(actionRequested == 1) 
            return 1;
      }
   }

   return 0;
}

int client::sendAll(int fd, unsigned char *msg, int msg_len) {
   int bytes_left = msg_len;
   int bytes_send = 0;
   while(bytes_left > 0) {
      if((bytes_send = send(fd, msg + msg_len - bytes_left, bytes_left, 0)) == -1)
         return -1;
      bytes_left = bytes_left - bytes_send;
   }
   return 0;
}

void client::setHolecards(int c1, int c2)
{
   me->setHolecards(c1, c2);
   _onFlop = false;
   _onTurn = false;
   _onRiver = false;
   emit updateTable();
}

void client::setFlop(int c1, int c2, int c3)
{
   flop[0] = c1;
   flop[1] = c2;
   flop[2] = c3;
   _onFlop = true;
   emit updateTable();
}

void client::setTurn(int c1)
{
   turn = c1;
   _onTurn = true;
   emit updateTable();
}

void client::setRiver(int c1)
{
   river = c1;
   _onRiver = true;
   emit updateTable();
}

void client::setPlayerName(int n, char *name) {
   player[n].setName(name);
}

void client::playerHolecards(int n, int c1, int c2) {
   player[n].setHolecards(c1, c2);
   emit updateTable();
}

void client::playerIsDealer(int n) {
   
}

void client::playerIsSmallBlind(int n) {
   player[n].bets(smallBlind);
}

void client::playerIsBigBlind(int n) {
   player[n].bets(bigBlind);
   toCall = bigBlind;
}

void client::setPlayerCount(int n) {
   playerCount = n;
}

void client::setSeatNumber(int n) {
   me = &player[n];
   seatNumber = n;
}

void client::setStartingChips(int amount) {
   for(int i = 0; i < playerCount; i++) {
      player[i].setChips(amount);
   }
}

void client::newRound() {
   minimumBet = bigBlind;
   for(int i = 0; i < playerCount; i++)
      player[i].newRound();
}
