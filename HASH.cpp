#include <openssl/rand.h>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <cstring>
#define BUFSIZE (1024*50)
#define BUFSIZE2 (32)

using namespace std;

int do_hash(char* infile)/*функция, создающая файл:хеши-блоки данных*/
{
	
	FILE *inf, *tmp;
	unsigned char mas[BUFSIZE];
	unsigned char buf[BUFSIZE];
	EVP_MD_CTX mdctx; /* контекст для вычисления хэша */
	const EVP_MD * md; /* структура с адресами функций алгоритма */
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; /* размер вычисленного хэша */ 
	inf = fopen(infile,"r+");/* В командной строке передаем имя файла, для которого вычисляется хэш */
	tmp = fopen("tmp.txt","w+");
	OpenSSL_add_all_digests();/* Добавляем алгоритмы хэширования во внутреннюю таблицу библиотеки */
	md = EVP_get_digestbyname("sha256");/* Получаем адреса функций алгоритма MD5 и инициализируем контекст для вычисления хэша */
	
	size_t read;
	/* Вычисляем хэш */
	for(;;) 
	{
		for(int i=0; i<BUFSIZE; ++i)
		buf[i]='\0';
		int i = fread(buf,1, BUFSIZE-1,inf);
		if(i <= 0) break;
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, buf, (unsigned long)i);
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);
		fwrite(md_value,1, md_len,tmp);
		fwrite(buf,1,strlen((const char *)buf),tmp);

	}
	fclose(inf);
	fclose(tmp);
	inf = fopen(infile,"w+");
	tmp = fopen("tmp.txt","r+");

	while((read = fread(mas,1, BUFSIZE,tmp))!= 0)
   		fwrite(mas,1,read,inf);
		
	fclose(inf);
	fclose(tmp);
	remove("tmp.txt");
	return 0;	
}

bool check_hash(char*infile)/*функция, проверяющая целостность данных*/
{
	FILE *inf, *tmp;
	char *mas;
	unsigned char buf[BUFSIZE];
	unsigned char buf2[BUFSIZE2];
	EVP_MD_CTX mdctx; /* контекст для вычисления хэша */
	const EVP_MD * md; /* структура с адресами функций алгоритма */
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; /* размер вычисленного хэша */ 
	 /* В командной строке передаем имя файла, для которого вычисляется хэш */
	inf = fopen(infile,"r+");
	OpenSSL_add_all_digests();
	md = EVP_get_digestbyname("sha256");
	int count = 0;
	for(;;) 
	{
		count ++;
		int k= fread(buf2,1, BUFSIZE2,inf);
		if( k<=0) break;
		for(int i=0; i<BUFSIZE; ++i)
			buf[i]='\0';

		int i = fread(buf,1, BUFSIZE,inf);
		if(i <= 0) break;
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, buf, (unsigned long)i);
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx); 
		count = 0;
		if (count = memcmp(buf2,md_value,32)) 
		{
			return 0;
		}
		
	}
	fclose(inf);
	return 1;

}

int do_hash2(char*infile)/*функция, записывающая вместо данных хеш*/
{
	FILE *inf;
	unsigned char buf[BUFSIZE];
	EVP_MD_CTX mdctx; /* контекст для вычисления хэша */
	const EVP_MD * md; /* структура с адресами функций алгоритма */
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; /* размер вычисленного хэша */ 
	inf = fopen(infile,"r+");/* В командной строке передаем имя файла, для которого вычисляется хэш */
	OpenSSL_add_all_digests();/* Добавляем алгоритмы хэширования во внутреннюю таблицу библиотеки */
	md = EVP_get_digestbyname("sha256");/* Получаем адреса функций алгоритма MD5 и инициализируем контекст для вычисления хэша */
	
	size_t read;
	/* Вычисляем хэш */
	for(;;) 
	{
		for(int i=0; i<BUFSIZE; ++i)
		buf[i]='\0';
		int i = fread(buf,1, BUFSIZE-1,inf);
		if(i <= 0) break;
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, buf, (unsigned long)i);
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);
		

	}
	fclose(inf);
	inf = fopen(infile,"w+");
	fwrite(md_value,1, md_len,inf);
		
	fclose(inf);
	return 0;	
}

int main(int argc, char**argv)
{ 
	unsigned char mas[32];
	FILE*in;

	//Нахождение хеша для файла(строчки в файле) и считывание его в буфер
	do_hash2(argv[1]);
	in=fopen(argv[1],"r+");
	int k= fread(mas,1, 32,in);
	do_hash(argv[1]);
	if (check_hash(argv[1])) cout<<"OK"<<endl;
	else cout<<"FAIL"<<endl;
	return 0;
} 