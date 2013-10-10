#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTR "20130822145144"

void echo_tm(const struct tm *tm){/*{{{*/

    printf("tm_year %d\n"
           "tm_mon %d\n"
           "tm_mday %d\n"
           "tm_hour %d\n"
           "tm_min %d\n"
           "tm_sec %d\n", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}/*}}}*/

time_t parse_time_str(char *timestr){/*{{{*/

    char ch[32];
    char *p = timestr;
    int i = 0, j = 0;
    int year, month, day, hour, minute, second;
    struct tm tm;

    for(; p[i]; i++){
        if(i - 3 > 0 && i % 2 == 0)
            ch[j++] = '-';
        ch[j++] = p[i];
    }
    ch[j++] = 0;

    year = strtoul(ch, &p, 10);
    if(year < 0)
        return -1;

    month = strtoul(++p, &p, 10);
    if(month > 12)
        return -1;
    
    day = strtoul(++p, &p, 10);
    if(day > 31)
        return -1;
    
    hour = strtoul(++p, &p, 10);
    if(hour > 24)
        return -1;
    
    minute = strtoul(++p, &p, 10);
    if(minute > 60)
        return -1;
    
    second = strtoul(++p, &p, 10);
    if(second > 60)
        return -1;

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    
    return mktime(&tm);
}/*}}}*/

int judge_time_ok(time_t t1, time_t t2){
    return difftime(t1, t2);
}

int main(void){

#if 0
    time_t xx;
    struct tm *kk;
    xx = time(NULL);
    kk = localtime(&xx);
    echo_tm(kk);

    xx = parse_time_str(TIMESTR);
    kk = localtime(&xx);
    echo_tm(kk);
#endif

    printf("ret ## %d\n", judge_time_ok(time(NULL), parse_time_str(TIMESTR)));
        
    return 0;
}
