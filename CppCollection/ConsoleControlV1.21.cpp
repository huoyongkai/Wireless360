
/**
 * @file
 * @brief Console Control
 * @version 1.20
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  JMar 4, 2010-July 14, 2011
 * @copyright None.
*/


#include "Headers.h"
#include ConsoleControl_H

struct termios ConsoleControl::s_initialsettings;
struct timeval ConsoleControl::s_tv;
fd_set ConsoleControl::s_rfds;
bool ConsoleControl::s_controlling=false;
bool ConsoleControl::InitUnblockGet()//single get unblocked
{
    struct termios newsettings;
    tcgetattr(fileno(stdin),&s_initialsettings);
    newsettings = s_initialsettings;
    //newsettings.c_lflag &= ~ECHO; /*disable echo*/
    newsettings.c_lflag &= ~ICANON;/*Enable canonical mode*/
    newsettings.c_cc[VMIN] = 1;/*VMIN等待最小的字符个数，设置为1，则每输入一个
    *字符就会送到程序。
    */
    newsettings.c_cc[VTIME] = 0;/*等待的最小时间，设施为0，则一直等待*/
    if (tcsetattr(fileno(stdin),TCSAFLUSH,&newsettings) !=0)
    {
	fprintf(stderr,"ConsoleControl::InitUnblockGet:Could not set attributes\n");
	s_controlling=false;
    }
    else {
	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&s_rfds);
	FD_SET(0, &s_rfds);
	/* Wait up to 0 seconds. no wait */
	s_tv.tv_sec =0;//seconds
	s_tv.tv_usec = 0;//Microseconds
	s_controlling=true;
    }
    return s_controlling;
}
bool ConsoleControl::EndUnblockGet()//single get unblocked
{
    if(s_controlling)
    {
	s_controlling=false;
	return tcsetattr(fileno(stdin),TCSANOW,&s_initialsettings)==0?true:false;//还原设置
    }
    else
	return false;
}
int ConsoleControl::GetChar()
{
    //s_tv.tv_sec=0;
    FD_ZERO(&s_rfds);
    FD_SET(0, &s_rfds);
    int retval = select(1, &s_rfds, NULL, NULL, &s_tv);
    /* Don't rely on the value of tv now! */
    if (retval == -1)
    {
	perror("select()");
	return -1;
    }
    else if (retval)
	return getchar();
    else
	return -1;
}
void ConsoleControl::Clear()
{
    // cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
    //cin.ignore();
    //setbuf(stdin, NULL); 只能清空当前已送入(stdin)的字符
    //flush(stdin); for windows
    //rewind(stdin);for windows
    tcflush(0, TCIFLUSH);//clear key board buf
}
int ConsoleControl::Gotoxy(int x, int y)
{
    char essq[MAX_SCREEN_AREA]={0}; // String variable to hold the escape sequence
    sprintf(essq, "\033[%d;%df", y,x);
    printf("%s", essq);
    return 0;
}
int ConsoleControl::BackNChars(int _n)
{
    for (int i=0;i<_n;i++)
    {
	cout<<"\b";
    }
    for (int i=0;i<_n;i++)
    {
	cout<<" ";
    }
    for (int i=0;i<_n;i++)
    {
	cout<<"\b";
    }
    cout.flush();
    return 0;
}
int ConsoleControl::GetXY(int& x,int& y)
{
    /*initscr();
    getyx(stdscr,y,x);
    refresh();
    endwin();*/
    return 0;
}
