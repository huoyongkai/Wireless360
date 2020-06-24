
/**
 * @file
 * @brief Console Control
 * @version 1.20
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  JMar 4, 2010-July 14, 2011
 * @copyright None.
*/

#ifndef CONSOLECONTROL_H_
#define CONSOLECONTROL_H_
//#include <curses.h>
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
using namespace std;
#define MAX_SCREEN_AREA 100
#define MAX_OUTPUT 100

/**
 * @brief screen control and setting class
 */
class ConsoleControl
{
private:
    static struct termios s_initialsettings;
    static struct timeval s_tv;
    static fd_set s_rfds;
    //! whether the calss is controlling a console
    static bool s_controlling;
public:
    //! get control permission of screen
    static bool InitUnblockGet();
    //! finish control of screen
    static bool EndUnblockGet();
    //! get a single char from input stream
    static int GetChar();
    //! clear settings
    static void Clear();
    //! goto position(x,y)
    static int Gotoxy(int x, int y);
    //! go back _n chars
    static int BackNChars(int _n);
    static int GetXY(int& x,int& y);
};
#endif /* CONSOLECONTROL_H_ */
#ifdef _programme_bak_here_
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
int main()
{//1 输入响应
    int c;
    struct termios initialisettings ,newsettings;
    tcgetattr(fileno(stdin),&initialisettings);
    newsettings = initialisettings;
    newsettings.c_lflag &= ~ECHO; /*disable echo*/
    newsettings.c_lflag &= ~ICANON;/*Enable canonical mode*/
    newsettings.c_cc[VMIN] = 1;/*VMIN等待最小的字符个数，设置为1，则每输入一个
                                                *字符就会送到程序。
                                                */
    newsettings.c_cc[VTIME] = 0;/*等待的最小时间，设施为0，则一直等待*/
    if (tcsetattr(fileno(stdin),TCSAFLUSH,&newsettings) !=0)
    {
        fprintf(stderr,"Could not set attributes\n");
    }
    else {
        while ((c=getchar()) != 'q')
        {
            printf("you press :%c\n",c);
        }
    }
    tcsetattr(fileno(stdin),TCSANOW,&initialisettings);//还原设置
    return 0;
}
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
int main(void)
{//非阻塞响应
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec =4;//seconds
    tv.tv_usec = 0;//Microseconds

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
    /* FD_ISSET(0, &rfds) will be true. */
    else
        printf("No data within five seconds.\n");
    cout<<tv.tv_sec<<endl;

    return 0;
}
#endif
