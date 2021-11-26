#include <iostream>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <cstring>

using namespace std;

#define LINESZ 2000

struct argList
{
	const char *argBuffer;
	int length;
	int curArgIdx;
	int curArgc;
} arglist;

void initArgList(const char *args)
{
	arglist.argBuffer = args;
	arglist.length = strlen(args);
	arglist.curArgIdx = 0;
	arglist.curArgc = 0;
}

int getNextArg(char *arg)
{
	int argLen, i;
	if (arglist.length <= arglist.curArgIdx)
		return 0;

	for (i = arglist.curArgIdx; i < arglist.length; i++)
	{
		if (arglist.argBuffer[i] == ' ')
		{
			break;
		}
	}
	argLen = i - arglist.curArgIdx;
	memcpy(arg, arglist.argBuffer + arglist.curArgIdx, argLen);
	arg[argLen] = '\0';
	arglist.curArgIdx += argLen + 1;
	return argLen;
}

int printArgs(WINDOW *w, const char *input)
{
	initArgList(input);
	char buf[1024];
	int curY = getcury(w)+1;
	int argCount = 0;

	while( getNextArg(buf) != 0 ) {
		if (curY >= getmaxy(w))
		{
			wmove(w, 1, 1);
			wdeleteln(w);
			curY--;
		}
		mvwprintw(w, curY++, 1, "arg %d : %s", argCount, buf);
		argCount ++;
	}
	return argCount;
}

int printInput(WINDOW *w, const char *input)
{
	char buf[4096] = {0, };
	int curIdx = 0;
	int maxX = getmaxx(w) - 1;
	int maxY = getmaxy(w) - 1;
	int length = 0;
	int curY = getcury(w);
	int printLine = 0;

	snprintf(buf, 4095, "input : %s", input);
	length = strlen(buf);
	while (curIdx < length) {
		
		mvwaddnstr(w, ++curY, 1, buf+curIdx, maxX - 1 );
		curIdx += maxX - 1;
		printLine ++;
	}
	return printLine;	
}

int cmd(WINDOW *w)
{
	char c;
	int idx = 0;
	char args[LINESZ];
	char buf[1024];
	bool bPrevSpace = false;
	bool bIsDone = false;
	int curY = 1;

	box(w, '|', '-');
	while (true)
	{
		bIsDone = false;
		idx = 0;
		if(curY >= getmaxy(w)) {
			wmove(w, 1, 1);
			wdeleteln(w);
			curY = getmaxy(w) - 1;
		}
		mvwprintw(w,curY, 1, "> ");
		while (true)
		{

			if (getcurx(w) >= getmaxx(w) - 1) {
				if (curY >= getmaxy(w))
				{
					wmove(w, 1, 1);
					wdeleteln(w);
					curY = getmaxy(w)-2;
				}
				wmove(w, ++curY, 1);
			}
			c = wgetch(w);
			if (idx > LINESZ && c != '\n')
				continue;
			switch (c)
			{
			case ' ':
			case '\t':
				if (!bPrevSpace && idx > 0)
				{
					args[idx++] = ' ';
					bPrevSpace = true;
				}

				break;

			case '\n':
				bIsDone = true;
				curY++;
				args[idx] = '\0';
				curY += printArgs(w, args);
				break;
			default:
				args[idx++] = c;
				bPrevSpace = false;
				break;
			}
			if (bIsDone)
				break;
		}
	}
}

int main(void)
{	
	WINDOW *testw;
	initscr();
	start_color();

	testw = newwin(16, 80, 1, 1);
	cmd(testw);

	
	endwin();
	return 0;
}
