#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "LHL_encrypt.h"

static void swap_byte(unsigned char *a, unsigned char *b){

	unsigned char swapByte; 
       
	swapByte = *a; 
	*a = *b;      
	*b = swapByte;
}

static void prepare_key(const unsigned char *key_data_ptr, int key_data_len, rc4_key *key){

	unsigned char index1;
	unsigned char index2;
	unsigned char* state;
	short counter;     
     
	state = &key->state[0];         
	
	for(counter = 0; counter < 256; counter++)              
		state[counter] = counter;

	key->x = 0;     
	key->y = 0;     
	index1 = 0;
	index2 = 0;  

	for(counter = 0; counter < 256; counter++){               
		index2 = (key_data_ptr[index1] + state[counter] + index2) % 256;                
		swap_byte(&state[counter], &state[index2]);            
		index1 = (index1 + 1) % key_data_len;  
	}       
}
    
static void rc4(unsigned char *buffer_ptr, int buffer_len,  rc4_key *key){
 
	unsigned char x;
	unsigned char y;
	unsigned char* state;
	unsigned char xorIndex;
	short counter;              
        
	x = key->x;     
	y = key->y;     
        
	state = &key->state[0];         
	
	for(counter = 0; counter < buffer_len; counter++){               
		x = (x + 1) % 256;                      
		y = (state[x] + y) % 256;               
		swap_byte(&state[x], &state[y]);                       // maybe I's beating the air 
		xorIndex = (state[x] + state[y]) % 256;                 
		buffer_ptr[counter] ^= state[xorIndex];         
	}               
}

void rc4Decrypt(char *key, char *data, int len){

	rc4_key rc4key;

	prepare_key(key, strlen(key), &rc4key); 
	rc4((unsigned char *)data, len, &rc4key);
}

//base64编码表 
static char base64_table[] = { 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', 
    '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
}; 

static char str_pad = '=';//pad 用“=”标记 

//内部申请内存，外部一定要记得释放内存
char *base64Encrypt(const unsigned char *str, int length) 
{ 
	char *result,*p; 
    
	if(str == NULL || length <1)
		return NULL; 
    //分配空间,加密后的字符串长度是原字符串的4/3 
    result = (char *)malloc(((length + 2) / 3) * 4 + 1);    // +1 for '\0' 
    p = result; 
    
	if(result == NULL){ 
    	printf("malloc failed\n"); 
    	return NULL; 
    } 
/**
 * 这是剩余字符串长度大于等于3的情况 
 * 通过位移来截取字节的位数 
 * 第一个字节右移2位，得到base64的第一个目标字符 
 * 第一个字节&0x03（00000011）后，再加上第二个字节右移4位，得到base64的第二个目标字符 
 * 第二个字节&0x0f (00001111) 后，再加上第三个字节右移4位，得到base64的第三个目标字符 
 * 第三个字节&0x3f (00111111) 后，得到base64的第四个目标字符 
**/    
	while( length > 2){ 
    	*result++ = base64_table[str[0] >> 2]; 
    	*result++ = base64_table[((str[0] & 0x03) << 4) + (str[1] >> 4)]; 
    	*result++ = base64_table[((str[1] & 0x0f) << 2) + (str[2] >> 6)]; 
    	*result++ = base64_table[str[2] & 0x3f]; 
    	length -= 3; 
    	str += 3; 
    } 
    //剩余字符串长度小于3 
    if(length != 0){ 
    	*result++ = base64_table[str[0] >> 2]; //剩余长度为2 
        if(length > 1){ 
        	*result++ = base64_table[((str[0] & 0x03) << 4) + (str[1] >> 4)]; 
        	*result++ = base64_table[(str[1] & 0x0f) << 2]; 
        	*result++ = str_pad;               //不够的补"=" 
        }else{                                 //剩余字符串长度是1 
        	*result++ = base64_table[(str[0] & 0x03) << 4]; 
        	*result++ = str_pad;               //不够的补"=" 
        	*result++ = str_pad;               //不够的补"=" 
        } 
    } 
    *result = 0; 
    //输出结果 
    return p; 
} 

  
/**
 * Base-64 decoding.  This represents binary data as printable ASCII
 * characters.  Three 8-bit binary bytes are turned into four 6-bit
 * values, like so:
 *
 * [11111111] [22222222] [33333333]
 *
 * [111111] [112222] [222233] [333333]
 *
 * Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
**/

static int b64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1  /* F0-FF */
};
  
/** 
 * Do base-64 decoding on a string. Ignore any non-base64 bytes.
 * Return the actual number of bytes generated. The decoded size will
 * be at most 3/4 the size of the encoded, and may be smaller if there
 * are padding characters (blanks, newlines).
**/

int base64Decrypt(const char *str, unsigned char* space, int size){

	const char* cp = NULL;
	int space_idx = 0, phase = 0;
	int d = 0, prev_d = 0;
	unsigned char c;

	for(cp = str; *cp != '\0'; ++cp){
		d = b64_decode_table[(int)*cp];
	
		if(d != -1){
			switch(phase){
				case 0:
					++phase;
					break;
				case 1:
					c = ((prev_d << 2) | ((d & 0x30) >> 4));
					if(space_idx < size)
						space[space_idx++] = c;
					++phase;
					break;
				case 2:
					c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
					if(space_idx < size)
						space[space_idx++] = c;
					++phase;
					break;
				case 3:
					c = (((prev_d & 0x03) << 6) | d);
					if(space_idx < size)
						space[space_idx++] = c;
					phase = 0;
					break;
			}
			prev_d = d;
		}
	}
	
	return space_idx;
}

int nas_Passwd_Encrypt(const char *passwd, char *buf, int bufsize){
	
	int datalen;
	char *str;

	if(passwd == NULL || buf == NULL)
		return -1;
	
	datalen = strlen(passwd);
	if(bufsize < (datalen + 2) / 3 * 4 + 1)
		return -1;

	strcpy(buf, passwd);

	rc4Decrypt(RC4_KEY, buf, datalen);

	if((str = base64Encrypt(buf, datalen)) == NULL)
		return -1;

	strcpy(buf, str);
	free(str);

	return 0;
}

int nas_Passwd_Decrypt(char *passwd, char *buf, int bufsize){
		
	int datalen;
	char *str;

	if(passwd == NULL || buf == NULL)
		return -1;
	
	datalen = strlen(passwd);
	if(bufsize < datalen / 4 * 3 + 1)
		return -1;

	datalen = base64Decrypt(passwd, buf, bufsize);

	rc4Decrypt(RC4_KEY, buf, datalen);
	buf[datalen] = 0;

	return 0;
}

#if 0
int main(void){

	char *k = "wocaozheshijieqwer1234omijialang";
	int n;
	char buf[1024];
	char ch[1024];

	if((n = nas_Passwd_Encrypt(k, buf, 1024)) != 0)
		puts("##################");
	puts(buf);

	if((n = nas_Passwd_Decrypt(buf, ch, 1024)) != 0)
		puts("******************");
	puts(ch);

	return 0;
}

//#define TEST_BASE64
#ifdef TEST_BASE64
int main(int argc, char *argv[]){
	
	char *encode;
	char buf[1024];
	char ch[1024];
	int fd, n;
	
	if((fd = open(argv[1], O_RDONLY)) == -1){
		puts("open err");
		exit(1);
	}

	n = read(fd, buf, 1024);
	printf("%d\n", n);
	encode = base64Encrypt(buf, n);
	puts(encode);
/*
	printf("%s",encode);
	fflush(stdout);	
*/
	if((fd = open("key4", O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1){
		puts("open err");
		exit(1);
	}
		
	n = base64Decrypt(encode, ch, 1024);
	write(fd, ch, n);

	return 0;
}
#endif

//#define TEST_RC4
#ifdef TEST_RC4
int main(int argc, char *argv[]){

	char buffer[200] = {0};
	int i, strl;
    char *Key = "keys"; 
	char *p;
	int len;

	if(argv[1] == NULL || (len = strlen(argv[1])) > 1024){
		printf("No argv[1] or argv[1] is longer than bufsize\n");
		return -1;	
	}
	
	memcpy(buffer, argv[1], len);

	printf("src string:\n");
	for( i = 0; i < strlen(buffer) ; i++){

		printf("%x ", buffer[i]);
	}
	printf("\n\n");

	rc4Decrypt(Key, buffer, len);

	printf("Encryption string:\n");
	for( i = 0; i < strlen(buffer) ; i++){

		printf("%x ", buffer[i]);
	}
	printf("\n\n");

	rc4Decrypt(Key, buffer, len);

	printf("Decryption string:\n");
	for( i = 0; i < strlen(buffer) ; i++){

		printf("%x ", buffer[i]);
	}
	printf("\n\n");

	return 0;
}
#endif

#endif
