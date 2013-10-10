#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main(void){

    char buf[1024];
    int n;
    struct termios initialrsettings, newrsettings;

#if 1
    tcgetattr(STDIN_FILENO, &initialrsettings);
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ECHO;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &newrsettings) != 0 ){
        fprintf(stderr,"Could not set arrributes\n");
    }else{
        puts("Password :");
        n = read(STDIN_FILENO, buf, 1024);
        write(STDOUT_FILENO, buf, n);
        tcsetattr(fileno(stdin),TCSANOW,&initialrsettings); 
    }
    
#endif
    return 0;
}
