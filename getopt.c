#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#define LDAP_PREFIX "ldap://"

typedef struct _ldap_cond{
    char *ad_uri;
    char *ad_manager;
    char *ad_passwd;
}win_ad_cond;

char *ad_dn;
win_ad_cond ad;

void usage(void){

    printf("Usage: \n"
           "\t-d ad_name\t domain name\n" 
           "\t-u ad_manager\t domain's manager account\n" 
           "\t-p ad_passwd\t domain's manager account password\n"
           "\t\t\t if ad_passwd is '-' cmd will read stdin to get password\n");

}

void free_ad_cond(void){

    if(ad.ad_uri)
        free(ad.ad_uri);
    if(ad.ad_manager)
        free(ad.ad_manager);
    if(ad.ad_passwd)
        free(ad.ad_passwd);
}

void dump_ad_cond(void){

    if(ad.ad_uri && ad.ad_manager && ad.ad_passwd)
        printf("ad_domain ## %s ##\n"
               "ad_manager ## %s ##\n"
               "ad_passwd ## %s ##\n", ad.ad_uri, ad.ad_manager, ad.ad_passwd);

}

#if 1
int opt_parse(int argc, char **argv, char *optstr){

    char c;
    char buf[1024];

    if(argc < 7){
        usage();
        exit(27);
    }

    strcpy(buf, LDAP_PREFIX);
    while((c = getopt(argc, argv, optstr)) != -1){
        switch(c){
            case 'd':
                strcat(buf, optarg);
                ad.ad_uri = strdup(buf);
                break;
            case 'u':
                ad.ad_manager = strdup(optarg);
                break;
            case 'p':
                ad.ad_passwd = strdup(optarg);
                break;
#if 0
            case 'n':
                printf("option ## %c ", c);
                printf("optarg ## %s ##\n", optarg);
                break;
            case 'f':
                printf("option ## %c ", c);
                printf("optarg ## %s ##\n", optarg);
                break;
#endif
            default :
                usage();
                free_ad_cond();
                break;
        }    
    }

    return 0;
}
#endif

int main(int argc, char *argv[]){
    
    int i = 0;

    opt_parse(argc, argv, "d:u:p:n:f");
    dump_ad_cond();
    
    return 0;
}
