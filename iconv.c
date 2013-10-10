#include <stdio.h>
#include <fcntl.h>
#include <iconv.h>
#include <unistd.h>

int gbk2utf_8(char *inbuf, int inbuf_n, char *outbuf, int outbuf_n){

    iconv_t cn;
    char *p_in = inbuf;
    char *p_out = outbuf;
    size_t len_in = inbuf_n;
    size_t len_out = outbuf_n;

    if((cn = iconv_open("utf-8//", "GBK//")) == (iconv_t)-1)
        return -1;

    if(iconv(cn, &p_in, &len_in, &p_out, &len_out) == -1){
		perror("iconv");
        return -1;
	}

    iconv_close(cn);

    return outbuf_n - len_out;
}

int main(){

    int fd;
    int n;
    char buf[1024];
    char buf2[1024];

    fd = open("kk", O_RDONLY);
    
    n = read(fd, buf, 1024);
    write(STDOUT_FILENO, buf, n);
    
    n = gbk2utf_8(buf, n, buf2, 1024);
    write(STDOUT_FILENO, buf2, n);

    return 0;
}
