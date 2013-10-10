#include "seat.h"

seat::seat() {
   inGame = true;
}

void seat::setName(char* _name)
{
   memcpy(name, _name, sizeof(_name));
}

void seat::setHolecards(int c1, int c2) {
   holecard[0] = c1;
   holecard[1] = c2;
   revealed = true;
}

void seat::bets(int n) {
   remainingChips -= n;
   currentBet += n;
}

void seat::wins(int n) {
   remainingChips += n;
}

void seat::folds() {
   folded = true;
}

void seat::allin() {
   bets(remainingChips);
   allined = true;
}

bool seat::hasFolded() {
   return folded;
}

void seat::newRound() {
   folded = false;
   allined = false;
   currentBet = 0;
   revealed = false;
}

seat::~seat() {}
