#include "dww.h"
#include <cmath>
#include <iostream>


void dww::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
    bool done = false;
    int tr, tc;
    Direction td;
/*
	switch(rank){
		case crown:cout << "crown\n";break;
		case knight:cout << "knight\n";break;
		case archer:cout << "archer\n";break;
		case infantry:cout << "infantry\n";break;
	}
*/
    while (!done) {
        tr = minR + rand() % (maxR - minR);
        tc = minC + rand() % (maxC - minC);
        if (sitrep.board[tr][tc].state == empty)done = true;
    }
    int rdist = ROWS / 2 - tr;
    int cdist = COLS / 2 - tc;
    if (abs(rdist) < abs(cdist)) {
        if (cdist > 0)td = toRight;
        else td = toLeft;
    } else {
        if (rdist > 0)td = up;
        else td = dn;
    }
    row = tr;
    col = tc;
    facing = td;
}

bool enemyChecksOut(int r, int c, string tla, SitRep s) {
    if (r < 0 || c < 0 || r >= ROWS || c >= COLS)return false;
    if (s.board[r][c].state != unit)return false;
    if (s.board[r][c].owner == tla)return false;
    //cout<<"pew!";
    return true;
}

bool archerEnemyInSights(int r, int c, Direction d, string tla, SitRep s, Action &a) {
    int r1, r2, r3, c1, c2, c3;
    if (d == up)r1 = r - 1;
    r2 = r - 2;
    r3 = r - 3;
    c1 = c - 1;
    c2 = c;
    c3 = c + 1;
    if (d == dn)r1 = r + 1;
    r2 = r + 2;
    r3 = r + 3;
    c1 = c - 1;
    c2 = c;
    c3 = c + 1;
    if (d == toLeft)r1 = r - 1;
    r2 = r;
    r3 = r + 1;
    c1 = c - 1;
    c2 = c - 2;
    c3 = c - 3;
    if (d == toRight)r1 = r - 1;
    r2 = r;
    r3 = r + 1;
    c1 = c + 1;
    c2 = c + 2;
    c3 = c + 3;
    a.action = attack;
    if (enemyChecksOut(r1, c1, tla, s)) {
        a.archerRow = r1;
        a.archerCol = c1;
        return true;
    }
    if (enemyChecksOut(r1, c2, tla, s)) {
        a.archerRow = r1;
        a.archerCol = c2;
        return true;
    }
    if (enemyChecksOut(r1, c3, tla, s)) {
        a.archerRow = r1;
        a.archerCol = c3;
        return true;
    }
    if (enemyChecksOut(r2, c1, tla, s)) {
        a.archerRow = r2;
        a.archerCol = c1;
        return true;
    }
    if (enemyChecksOut(r2, c2, tla, s)) {
        a.archerRow = r2;
        a.archerCol = c2;
        return true;
    }
    if (enemyChecksOut(r2, c3, tla, s)) {
        a.archerRow = r2;
        a.archerCol = c3;
        return true;
    }
    if (enemyChecksOut(r3, c1, tla, s)) {
        a.archerRow = r3;
        a.archerCol = c1;
        return true;
    }
    if (enemyChecksOut(r3, c2, tla, s)) {
        a.archerRow = r3;
        a.archerCol = c2;
        return true;
    }
    if (enemyChecksOut(r3, c3, tla, s)) {
        a.archerRow = r3;
        a.archerCol = c3;
        return true;
    }
    return false;
}

bool archerEnemyInRange(int r, int c, Direction d, string tla, SitRep s, Action &a) {
    int r1, r2, r3, c1, c2, c3;
    a.action = turn;
    //look up
    a.dirToFace = up;
    r1 = r - 1;
    r2 = r - 2;
    r3 = r - 3;
    c1 = c - 1;
    c2 = c;
    c3 = c + 1;
    if (enemyChecksOut(r1, c1, tla, s)) { return true; }
    if (enemyChecksOut(r1, c2, tla, s)) { return true; }
    if (enemyChecksOut(r1, c3, tla, s)) { return true; }
    if (enemyChecksOut(r2, c1, tla, s)) { return true; }
    if (enemyChecksOut(r2, c2, tla, s)) { return true; }
    if (enemyChecksOut(r2, c3, tla, s)) { return true; }
    if (enemyChecksOut(r3, c1, tla, s)) { return true; }
    if (enemyChecksOut(r3, c2, tla, s)) { return true; }
    if (enemyChecksOut(r3, c3, tla, s)) { return true; }
    //look down
    a.dirToFace = dn;
    r1 = r + 1;
    r2 = r + 2;
    r3 = r + 3;
    c1 = c - 1;
    c2 = c;
    c3 = c + 1;
    if (enemyChecksOut(r1, c1, tla, s)) { return true; }
    if (enemyChecksOut(r1, c2, tla, s)) { return true; }
    if (enemyChecksOut(r1, c3, tla, s)) { return true; }
    if (enemyChecksOut(r2, c1, tla, s)) { return true; }
    if (enemyChecksOut(r2, c2, tla, s)) { return true; }
    if (enemyChecksOut(r2, c3, tla, s)) { return true; }
    if (enemyChecksOut(r3, c1, tla, s)) { return true; }
    if (enemyChecksOut(r3, c2, tla, s)) { return true; }
    if (enemyChecksOut(r3, c3, tla, s)) { return true; }
    //look left
    a.dirToFace = toLeft;
    r1 = r - 1;
    r2 = r;
    r3 = r + 1;
    c1 = c - 1;
    c2 = c - 2;
    c3 = c - 3;
    if (enemyChecksOut(r1, c1, tla, s)) { return true; }
    if (enemyChecksOut(r1, c2, tla, s)) { return true; }
    if (enemyChecksOut(r1, c3, tla, s)) { return true; }
    if (enemyChecksOut(r2, c1, tla, s)) { return true; }
    if (enemyChecksOut(r2, c2, tla, s)) { return true; }
    if (enemyChecksOut(r2, c3, tla, s)) { return true; }
    if (enemyChecksOut(r3, c1, tla, s)) { return true; }
    if (enemyChecksOut(r3, c2, tla, s)) { return true; }
    if (enemyChecksOut(r3, c3, tla, s)) { return true; }
    //look right
    a.dirToFace = toRight;
    r1 = r - 1;
    r2 = r;
    r3 = r + 1;
    c1 = c + 1;
    c2 = c + 2;
    c3 = c + 3;
    if (enemyChecksOut(r1, c1, tla, s)) { return true; }
    if (enemyChecksOut(r1, c2, tla, s)) { return true; }
    if (enemyChecksOut(r1, c3, tla, s)) { return true; }
    if (enemyChecksOut(r2, c1, tla, s)) { return true; }
    if (enemyChecksOut(r2, c2, tla, s)) { return true; }
    if (enemyChecksOut(r2, c3, tla, s)) { return true; }
    if (enemyChecksOut(r3, c1, tla, s)) { return true; }
    if (enemyChecksOut(r3, c2, tla, s)) { return true; }
    if (enemyChecksOut(r3, c3, tla, s)) { return true; }
    return false;
}


// tell someone what you want to do
Action dww::Recommendation(SitRep sitrep) {

    // this code is provided as an example only
    // use at your own risk
    Action a;

    //if I am the crown, bravely run away
    if (rank == crown) {
        Direction mydir;
        switch (sitrep.nearestEnemy.direction) {
            case up:
                mydir = dn;
                break;
            case dn:
                mydir = up;
                break;
            case toLeft:
                mydir = toRight;
                break;
            case toRight:
                mydir = toLeft;
                break;
            case none:
                mydir = none;
                break;
        }
        if (facing == mydir) {
            a.action = moveFwd;
            a.maxDist = HORSESPEED / 2;
            return a;
        } else {
            a.action = turn;
            a.dirToFace = mydir;
            return a;
        }
    }


    //if I am an archer, look for a target in my sights
    if (rank == archer && archerEnemyInSights(row, col, facing, tla, sitrep, a))return a;
    //if I am an archer, look for a target in my range
    if (rank == archer && archerEnemyInRange(row, col, facing, tla, sitrep, a))return a;


    // first, try to attack in front of you
    int tr = row, tc = col;
    switch (facing) {
        case up:
            tr--;
            break;
        case dn:
            tr++;
            break;
        case toRight:
            tc++;
            break;
        case toLeft:
            tc--;
            break;
        case none:
            break;
    }
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS) {
        if (sitrep.board[tr][tc].state == unit) {
            if (sitrep.board[tr][tc].owner != tla) {
                a.action = attack;
                a.archerRow = tr;
                a.archerCol = tc;
                return a;
            }
        }
    }
    // there is not an enemy in front of me
    // so head to the nearest enemy
    if (facing == sitrep.nearestEnemyCrown.direction) {
        a.action = moveFwd;
        a.maxDist = 1;
        if (rank == knight || rank == crown)a.maxDist = HORSESPEED / 2;
        return a;
    } else {
        a.action = turn;
        a.dirToFace = sitrep.nearestEnemyCrown.direction;
        return a;
    }


    a.action = nothing;
    return a;

}
