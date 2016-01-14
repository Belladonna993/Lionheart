#include "sab.h"
#include <cmath>
#include <iostream>


void sab::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
    bool done = false;
    int targetRow, targetCol;
    Direction toFace;


    //put Crown far from enemy (Note that Lionheart.cpp places the crown first; otherwise, placing the while inside the else loop
    //would be a little broken, or at least the crown would end up being placed randomly, as well. Possibly the code should be
    // more robust.)
    if (rank == crown) {
        if (maxC < COLS / 2) {
            //find the first empty space in minC
            for (int i = minR; i < maxR; i++) {
                if (sitrep.board[minC][i].state == empty) {
                    targetCol = minC;
                    targetRow = i;
                    done = true;
                    break;
                }
            }
        } else {
            //find the first empty space in maxC
            for (int i = minR; i < maxR; i++) {
                if (sitrep.board[maxC][i].state == empty) {
                    targetCol = maxC;
                    targetRow = i;
                    done = true;
                    break;
                }
            }
        }
    }

    //attempt to place some knights in front row

 /*   if (rank == knight) {
        if (maxC < COLS / 2) {
            //see if there's an empty space in maxC
            for (int i = minR; i < maxR; i++) {
                if (sitrep.board[maxC][i].state == empty) {
                    targetCol = maxC;
                    targetRow = i;
                    done = true;
                    break;
                }
            }
        } else {
            //see if there's an empty space in minC
            for (int i = minR; i < maxR; i++) {
                if (sitrep.board[minC][i].state == empty) {
                    targetCol = minC;
                    targetRow = i;
                    done = true;
                    break;
                }
            }
        }
    }*/

    //place the remaining pieces randomly
    while (!done) {
        targetRow = minR + rand() % (maxR - minR);
        targetCol = minC + rand() % (maxC - minC);
        if (sitrep.board[targetRow][targetCol].state == empty)done = true;
    }


    int rdist = ROWS / 2 - targetRow;
    int cdist = COLS / 2 - targetCol;
    if (abs(rdist) < abs(cdist)) {
        if (cdist > 0)toFace = toRight;
        else toFace = toLeft;
    } else {
        if (rdist > 0)toFace = up;
        else toFace = dn;
    }
    row = targetRow;
    col = targetCol;
    facing = toFace;
}


// tell someone what you want to do
Action sab::Recommendation(SitRep sitrep) {
    int targetRow = row, targetCol = col;
    Action action;


    //Set target row and col for movement for all units and attack for all but archer units
    switch (facing) {
        case up:
            targetRow--;
            break;
        case dn:
            targetRow++;
            break;
        case toRight:
            targetCol++;
            break;
        case toLeft:
            targetCol--;
            break;
        case none:
            break;
    }

    if (rank == crown) {
        //The crown tries to hide or run away
/*        if (abs(sitrep.nearestEnemy.col - col) > 10 && abs(sitrep.nearestEnemy.row - row) > 10) {
            action.action = nothing;
            return action;
        }*/
        //Turn toward an empty space if blocked
        if (targetRow < 0 || targetRow >= ROWS || targetCol < 0 || targetCol >= COLS ||
            sitrep.board[targetRow][targetCol].state != empty) {
            if (sitrep.anyOpenSpace != none) {
                action.dirToFace = sitrep.anyOpenSpace;
                action.action = turn;
                return action;
            } else {
                //attack out of desperation (I wish I knew how to make a separate method for this code since I'll be
                // reusing it.)
                if (targetRow >= 0 && targetRow < ROWS && targetCol >= 0 && targetCol < COLS) {
                    if (sitrep.board[targetRow][targetCol].state == unit) {
                        if (sitrep.board[targetRow][targetCol].owner != tla) {
                            action.action = attack;
                            action.archerRow = targetRow;
                            action.archerCol = targetCol;
                            return action;
                        }
                    }
                }
            }

        } else { //Run away Sir Robin!
            //Don't run TOWARD the enemy (although the unit could get stuck, think about improving this)
            Direction toFace;
            if (facing == sitrep.nearestEnemy.direction) {
                switch (sitrep.nearestEnemy.direction) {
                    case up:
                        toFace = dn;
                        break;
                    case dn:
                        toFace = up;
                        break;
                    case toRight:
                        toFace = toLeft;
                        break;
                    case toLeft:
                        toFace = toRight;
                        break;
                    default:
                        toFace = facing;
                }
                action.dirToFace = toFace;
                action.action = turn;
                return action;
            }
            //Okay, now run as fast as you can
            action.maxDist = HORSESPEED;
            action.action = moveFwd;
            return action;
        }
    }


    if (rank == infantry || rank == knight) {

        if (targetRow >= 0 && targetRow < ROWS && targetCol >= 0 && targetCol < COLS) {
            if (sitrep.board[targetRow][targetCol].state == unit) {
                if (sitrep.board[targetRow][targetCol].owner != tla) {
                    action.action = attack;
                    action.archerRow = targetRow;
                    action.archerCol = targetCol;
                    return action;
                }
            }
        }

        //Knights go after the crown
        if (rank == knight) {
            if (facing == sitrep.nearestEnemyCrown.direction) {
                action.action = moveFwd;
                if (rank == knight)action.maxDist = HORSESPEED;
                return action;
            } else {
                action.action = turn;
                action.dirToFace = sitrep.nearestEnemyCrown.direction;
                return action;
            }
        }
            //Infantry just tries to annihilate whatever
        else if (facing == sitrep.nearestEnemy.direction) {
            action.action = moveFwd;
            action.maxDist = 1;
            return action;
        } else {
            action.action = turn;
            action.dirToFace = sitrep.nearestEnemy.direction;
            return action;
        }
    }

    if (rank == archer) {


        //Set archer attack bounds
        int minAttackRow, maxAttackRow, minAttackCol, maxAttackCol;
        switch (facing) {
            case up:
                minAttackRow = row - 3;
                maxAttackRow = row - 1;
                minAttackCol = col - 1;
                maxAttackCol = col + 1;
                break;
            case dn:
                minAttackRow = row + 1;
                maxAttackRow = row + 3;
                minAttackCol = col - 1;
                maxAttackCol = col + 1;
                break;
            case toLeft:
                minAttackRow = row - 1;
                maxAttackRow = row + 1;
                minAttackCol = col - 3;
                maxAttackCol = col - 1;
                break;
            case toRight:
                minAttackRow = row - 1;
                maxAttackRow = row + 1;
                minAttackCol = col + 1;
                maxAttackCol = col + 3;
                break;
            case none:
                break;
        }

        //Adjust for board edges
        if (minAttackRow < 0) {
            maxAttackRow = 0;
        }

        if (minAttackCol < 0) {
            minAttackCol = 0;
        }

        if (maxAttackRow >= ROWS) {
            maxAttackRow = ROWS - 1;
        }

        if (maxAttackCol >= COLS) {
            maxAttackCol = COLS - 1;
        }

        //Try to attack the crown
        if (facing == sitrep.nearestEnemyCrown.direction && sitrep.nearestEnemyCrown.row >= minAttackRow &&
            sitrep.nearestEnemyCrown.row <= maxAttackRow && sitrep.nearestEnemyCrown.col >= minAttackCol &&
            sitrep.nearestEnemyCrown.col <= maxAttackCol) {

            action.action = attack;
            action.archerRow = sitrep.nearestEnemyCrown.row;
            action.archerCol = sitrep.nearestEnemyCrown.col;
            return action;
        }

        //If I can't attack the crown, figure out how many viable targets I have
        int enemyTargets = 0;
        int targetAttackRow = 0;
        int targetAttackCol = 0;
        for (int i = minAttackRow; i < maxAttackRow; i++) {
            for (int j = minAttackCol; j < maxAttackCol; j++) {
                if (sitrep.board[i][j].state == unit && sitrep.board[i][j].owner != tla) {
                    enemyTargets++;
                    targetAttackRow = i;
                    targetAttackCol = j;
                }
            }
        }

        //If I can only attack one enemy, go ahead and attack it
        if (enemyTargets == 1) {
            action.action = attack;
            action.archerRow = targetAttackRow;
            action.archerCol = targetAttackCol;
            return action;
        }

        //If I can attack more than one enemy, attack the one with lowest hit points (or, all things being equal, attack last target set in previous loop).
        int lowestHp = 10;
        if (enemyTargets > 1) {
            for (int i = minAttackRow; i < maxAttackRow; i++) {
                for (int j = minAttackCol; j < maxAttackCol; j++) {
                    if (sitrep.board[i][j].state == unit && sitrep.board[i][j].owner != tla &&
                        sitrep.board[i][j].hp < lowestHp) {
                        lowestHp = sitrep.board[i][j].hp;
                        targetAttackRow = i;
                        targetAttackCol = j;
                    }
                }
            }

            action.action = attack;
            action.archerRow = targetAttackRow;
            action.archerCol = targetAttackCol;
            return action;
        }

/*        //If I can't attack any enemies, try to move toward the crown
        if (facing == sitrep.nearestEnemyCrown.direction) {
            action.action = moveFwd;
            action.maxDist = 1;
            return action;
        } else {
            action.action = turn;
            action.dirToFace = sitrep.nearestEnemyCrown.direction;
            return action;
        }*/

        //If I can't attack any enemies, move toward one
        if (facing == sitrep.nearestEnemy.direction) {
            action.action = moveFwd;
            action.maxDist = 1;
            return action;
        } else {
            action.action = turn;
            action.dirToFace = sitrep.nearestEnemy.direction;
            return action;
        }

    }

    action.action = nothing;
    return action;
}





