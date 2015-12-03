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

int do_hash_for_file(char* infile,int id)/*функция, создающая файл:хеши-блоки данных*/
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
	//OpenSSL_add_all_digests();/* Добавляем алгоритмы хэширования во внутреннюю таблицу библиотеки */
	md = EVP_get_digestbynid(id);/* Получаем адреса функций алгоритма SHA256 и инициализируем контекст для вычисления хэша */
	//cout<<md->type<<endl;
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

bool check_hash_for_file(char*infile,int id)/*функция, проверяющая целостность данных*/
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
	//OpenSSL_add_all_digests();
	md = EVP_get_digestbynid(id);
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
		if (count = memcmp(buf2,md_value,BUFSIZE2)) 
		{
			return 0;
		}
		
	}
	fclose(inf);
	return 1;

}


void do_hash_for_str(const char*a, unsigned char*mas,int id)//функция,создающая хеш для строки
{
	
	EVP_MD_CTX mdctx; /* контекст для вычисления хэша */
	const EVP_MD * md; /* структура с адресами функций алгоритма */
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; /* размер вычисленного хэша */ 
	//OpenSSL_add_all_digests();/* Добавляем алгоритмы хэширования во внутреннюю таблицу библиотеки */
	md = EVP_get_digestbynid(id);/* Получаем адреса функций алгоритма SHA256 и инициализируем контекст для вычисления хэша */
	
	/* Вычисляем хэш */
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, a, (unsigned long)sizeof(a));
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);

		for(int i=0; i<BUFSIZE2; ++i)
			mas[i]=md_value[i];	
}

int main(int argc, char**argv)
{ 
	OpenSSL_add_all_digests();
	unsigned char mas[BUFSIZE2];
	//const char*a="BBByyyuh";
	const EVP_MD *md = EVP_get_digestbyname("sha256");
	int nid=md->type;

	//do_hash_for_str(a,mas,nid);
	//for(int i=0; i<BUFSIZE2; ++i)
		//cout<<mas[i];
	//cout<<endl;
	do_hash_for_file(argv[1],nid);
	if (check_hash_for_file(argv[1], nid)) cout<<"OK"<<endl;
	else cout<<"FAIL"<<endl;
	return 0;
} 
