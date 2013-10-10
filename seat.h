#include <inttypes.h>
#include <string.h>

class seat {

public:
   seat();

   void setChips(int chips) { remainingChips = chips; };

   void newRound();

   void setHolecards(int c1, int c2);

   int *getHolecards() { return holecard; };

   int getRemainingChips() { return remainingChips; };

   int getCurrentBet() { return currentBet; };

   void bets(int n);

   void wins(int n);

   void folds();

   void allin();

   bool hasFolded();

   bool isAllin() { return allined; };

   bool isInGame() { return inGame; };

   void setInGame(bool b) { inGame = b; };

   void setName(char *_name);

   char* getName() { return name; };

   bool cardsOnTable() { return revealed; }

   ~seat();

private:
   char name[32];

   bool revealed;

   int fd;

   int holecard[2];

   int currentBet;

   int remainingChips;

   bool folded, allined, inGame;

};
