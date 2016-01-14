#ifndef LIONHEART_H
#define LIONHEART_H
#include "unit.h"

#include "drw.h"
#include "dww.h"
#include "smb.h"
#include "zzz.h"
#include "sab.h"

#include "lionheart2.h"

using namespace std;

struct Box{
	int minr;
	int maxr;
	int minc;
	int maxc;
};

bool noPlay[NUMTLAS]={
0, 0, 0, 0, 0,
};

static const string tlalist[NUMTLAS]={
"sab",
"drw",
"dww",
"smb",
"zzz",
};
#endif
