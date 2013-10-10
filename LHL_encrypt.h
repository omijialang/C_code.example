#ifndef __NAS_ENCRYPT_H_
#define __NAS_ENCRYPT_H_

#define RC4_KEY "riernas"

typedef struct rc4_key_str{
      
	unsigned char state[256];       
	unsigned char x;        
	unsigned char y;
}rc4_key;

void rc4Decrypt(char *key, char *data, int len);
char *base64Encrypt(const unsigned char *str, int length);
int base64Decrypt(const char *str, unsigned char* space, int size);
int nas_Passwd_Encrypt(const char *passwd, char *buf, int bufsize);
int nas_Passwd_Decrypt(char *passwd, char *buf, int bufsize);

#endif
