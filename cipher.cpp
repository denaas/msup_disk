#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include "types.hpp"
using namespace std;
 
#define BUFSIZE 1024

// function for creating key for master key cipher using password
// and information about USB-flash
void PBKDF2_HMAC_SHA512(const char* password, const unsigned char* salt,
     int iterations, unsigned int keylen, unsigned char* out)
{
    const EVP_MD *digest;
    digest=EVP_sha512();
    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, strlen((char *)salt),
     iterations, digest, keylen, out);
}
 
int do_crypt_file(char *infile, unsigned char *key)
{
	const char *outfile="tmp";//temporary file for ciphered data
	int outlen, inlen;
	FILE *in, *out;
	unsigned char iv[8]={0,0,0,0,0,0,0,0}; // вектор инициализации 
	unsigned char inbuf[BUFSIZE], outbuf[BUFSIZE];
	EVP_CIPHER_CTX ctx;// контекст алгоритма шифрования
	const EVP_CIPHER * cipher;
	
	//open files
	if(!(in=fopen(infile,"r"))) cout<<"Error open file "<<infile<<endl;
	if(!(out=fopen(outfile,"w"))) cout<<"Error open file "<<outfile<<endl;
 
	// Обнуляем структуру контекста 
	EVP_CIPHER_CTX_init(&ctx);
 
	// Выбираем алгоритм шифрования 
	cipher = EVP_aes_256_ctr();
 
	// Инициализируем контекст алгоритма 
	EVP_EncryptInit(&ctx, cipher, key, iv);
 
	// Шифруем данные 
	for(;;) 
	{
		inlen = fread(inbuf, 1, BUFSIZE, in);
		if(inlen <= 0) break;
		if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) return 0;
		fwrite(outbuf, 1, outlen, out);// write crypt data in outfile
	}
 
	if(!EVP_EncryptFinal(&ctx, outbuf, &outlen)) return 0;
	fwrite(outbuf, 1, outlen, out);
	fclose(in);//close files
	fclose(out);
	EVP_CIPHER_CTX_cleanup(&ctx);// clean structure of cipher context
	remove(infile);// replace source file by ciphered file
	rename(outfile,infile);
	return 1;
}

int do_decrypt_file(char *infile, unsigned char *key)
{
	// Объявляем переменные 
	int outlen, inlen;// length of read and written data
	FILE *in, *out;
	const char *outfile="tmp";
	unsigned char inbuf[BUFSIZE], outbuf[BUFSIZE];
	EVP_CIPHER_CTX ctx;// context structure
	const EVP_CIPHER * cipher;// structure for cipher algorithm
	unsigned char iv[8]={0,0,0,0,0,0,0,0}; // initialize vector
 
	// Обнуляем контекст и выбираем алгоритм дешифрования 
	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_aes_256_ctr();// initialize structure for cipher algorithm
	EVP_DecryptInit(&ctx, cipher, key, iv);
 
	// Открываем входной и создаем выходной файлы 
	if(!(in=fopen(infile,"r"))) cout<<"Error open file "<<infile<<endl;
	if(!(out=fopen(outfile,"w"))) cout<<"Error open file "<<outfile<<endl;
	
	// Дешифруем данные 
	for(;;) 
	{
		inlen = fread(inbuf, 1, BUFSIZE, in);
		if(inlen <= 0) break;
 
		if(!EVP_DecryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) 
			return 0;
		fwrite(outbuf, 1, outlen, out);
	}
 
	// Завершаем процесс дешифрования, дешифруем оставшиеся биты
	if(!EVP_DecryptFinal(&ctx, outbuf, &outlen)) return 0;
	remove(infile);
	rename(outfile,infile);
 
}

int do_crypt_master_key(unsigned char *in, unsigned char *out, unsigned char *key)
{
	int outlen, inlen;
	unsigned char iv[8]={0,0,0,0,0,0,0,0}; // вектор инициализации 
	EVP_CIPHER_CTX ctx;// контекст алгоритма шифрования
	const EVP_CIPHER * cipher;
	// Обнуляем структуру контекста 
	EVP_CIPHER_CTX_init(&ctx);
 
	// Выбираем алгоритм шифрования 
	cipher = EVP_aes_192_ctr();
 
	// Инициализируем контекст алгоритма 
	EVP_EncryptInit(&ctx, cipher, key, iv);
 
	// Шифруем данные 
	inlen = strlen( (char*) in);
	if(!EVP_EncryptUpdate(&ctx, out, &outlen, in, inlen)) return 0; 
	if(!EVP_EncryptFinal(&ctx, out, &outlen)) return 0;
	
	EVP_CIPHER_CTX_cleanup(&ctx);// clean structure of cipher context
	return 1;
}

int do_decrypt_master_key(unsigned char *in, unsigned char *out, unsigned char *key)
{
	// Объявляем переменные 
	int outlen, inlen;// length of read and written data
	EVP_CIPHER_CTX ctx;// context structure
	const EVP_CIPHER * cipher;// structure for cipher algorithm
	unsigned char iv[8]={0,0,0,0,0,0,0,0}; // initialize vector
 
	// Обнуляем контекст и выбираем алгоритм дешифрования 
	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_aes_192_ctr();// initialize structure for cipher algorithm
	EVP_DecryptInit(&ctx, cipher, key, iv);
	
	// Дешифруем данные 
	inlen = strlen((char*)in);
	if(!EVP_DecryptUpdate(&ctx, out, &outlen, in, inlen)) 
		return 0;
	// Завершаем процесс дешифрования, дешифруем оставшиеся биты
	if(!EVP_DecryptFinal(&ctx, out, &outlen)) return 0;
	EVP_CIPHER_CTX_cleanup(&ctx);// clean structure of cipher context
}

void print(unsigned char* buf)
{
	for (unsigned int i = 0; i < sizeof(buf); i++)
     printf("%02x", 255 & buf[i]);
     printf("\n");
}

int main(int argc, char** argv)
{
	if (argc!=2)
	{
		cout<<"Usage: "<<argv[0]<<" <in_file.txt> "<<endl;
		return 0;
	}
	unsigned char key[32]; // 256 bits master key 
	unsigned char key_dec[32];
	TokenStructure token;
	unsigned char mk_cipher_key[24];//192 bits key for cipher master key
	const char* password = "password";
	const char *salt = "salt";
	unsigned char salt_hash[32];
	int keylen = 24;
	int iter = 4096;
	
	RAND_bytes(key, sizeof(key));
	cout<<"Master key : "<<key<<endl;
	print(key);
	
	RAND_bytes(salt_hash, sizeof((char*)salt_hash));
	// build key for master key cipher
	PBKDF2_HMAC_SHA512(password, salt_hash, iter, keylen, mk_cipher_key);
	//cout<<"PBKDF2 : "<<mk_cipher_key<<endl;
	//print(mk_cipher_key);
	do_crypt_master_key(key,token.key,mk_cipher_key);
	//cout<<"Ciphered master_key : "<<token.key<<endl;
	//print(token.key);
	//delete [] key;
	// form structure for writing on token
	//token.MKeyCipherAlg=;
	//token.NumOfIterarions=iter;
	//token.DataCipherAlg=;
	//token.PrfFunction=;
	// derive information for token integrity control
	//token.IntegrityCont=;
	do_decrypt_master_key(token.key,key_dec,mk_cipher_key);
	//cout<<"Decrypt key : "<<key_dec<<endl;
	//print(key_dec);
	if (!memcmp(key, key_dec, 32)) cout<<"YES\n"; else cout<<"NO\n";
	do_crypt_file(argv[1], key);	
	do_decrypt_file(argv[1], key_dec);
	return 0;
}
