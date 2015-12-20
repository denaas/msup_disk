#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include "types.hpp"
using namespace std;
 
#define BUFSIZE (1024*50)
#define BUFSIZE2 (32)

void TokenStructure::print(const char *file)
{
	FILE *in;
	if(!(in=fopen(file,"w+"))) cout<<"Error open file "<<file<<endl;
	fwrite(key, 1, 32, in);
	//fwrite(key, 1, 32, cout);
	fprintf(in," %d %d %d %d ", MKeyCipherAlg, NumOfIterations, DataCipherAlg,
		PrfFunction);
	fwrite(IntegrityCont,1, 32, in);
}

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


// function for creating key for master key cipher using password
// and information about USB-flash
void PBKDF2_HMAC_SHA256(const char* password, const unsigned char* salt,
    int salt_size, int iterations, unsigned int keylen, unsigned char* out)
{
    const EVP_MD *digest;
    digest=EVP_sha256();
    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, salt_size,
     iterations, digest, keylen, out);
}
 
int do_crypt_file(char *infile, unsigned char *key, const int nid)
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
	cipher = EVP_get_cipherbynid(906);
 
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

int do_crypt_master_key(unsigned char *in, int inlen, unsigned char *out, 
	const unsigned char *key, int nid)
{
	int outlen=0; 
	unsigned char iv[8]={0,0,0,0,0,0,0,0}; // вектор инициализации 
	EVP_CIPHER_CTX ctx;// контекст алгоритма шифрования
	const EVP_CIPHER * cipher;
	// Обнуляем структуру контекста 
	EVP_CIPHER_CTX_init(&ctx);
	// Выбираем алгоритм шифрования 
	cipher = EVP_get_cipherbynid(nid);
	// Инициализируем контекст алгоритма 
	EVP_EncryptInit(&ctx, cipher, key, iv);
	// Шифруем данные 
	if(!EVP_EncryptUpdate(&ctx, out, &outlen, in, inlen)) return 0; 
	if(!EVP_EncryptFinal(&ctx, out, &outlen)) return 0;
	
	EVP_CIPHER_CTX_cleanup(&ctx);// clean structure of cipher context
	return 1;
}

void do_hash_for_str(const char*a, unsigned char*mas,int nid)//функция,создающая хеш для строки
{
	
	EVP_MD_CTX mdctx; // контекст для вычисления хэша 
	const EVP_MD * md; // структура с адресами функций алгоритма 
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; // размер вычисленного хэша 
	//OpenSSL_add_all_digests();// Добавляем алгоритмы хэширования во внутреннюю таблицу библиотеки 
	md = EVP_get_digestbynid(nid);// Получаем адреса функций алгоритма SHA256 и инициализируем контекст для вычисления хэша 
	
	// Вычисляем хэш 
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, a, (unsigned long)sizeof(a));
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);

		for(int i=0; i<BUFSIZE2; ++i)
			mas[i]=md_value[i];	
}

void hmac_sha256(const unsigned char* in, int inlen, unsigned char* out, 
				unsigned char* key, int nid)
{
    unsigned int len = 32;
 	//unsigned char* result;
    //result = (unsigned char*)malloc(sizeof(char) * len);
    const EVP_MD * md;
 	md = EVP_get_digestbynid(nid);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);// clear structure
    // Using sha256 hash engine here.
    HMAC_Init(&ctx, key, 32, md);
    HMAC_Update(&ctx, (unsigned char*) in, inlen);
    HMAC_Final(&ctx, out, &len);
    HMAC_CTX_cleanup(&ctx);
 
   /* printf("HMAC digest: ");// print the result to check
 
    for (int i = 0; i != len; i++)
        printf("%02x", (unsigned int)out[i]);
 
    printf("\n");*/
 
    //free(result);
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
	OpenSSL_add_all_algorithms();// load information about all cipher algorithms
	unsigned char key[32]; // 256 bits master key 
	unsigned char key_dec[32];// decrypt aster key
	int mkey_len=32;// master key length
	TokenStructure token;
	unsigned char mk_cipher_key[32];//256 bits key for cipher master key
	const char* password = "password";
	const char *salt = "salt";
	unsigned char salt_hash[32];
	int keylen = 32;// length of key for master key cipher
	int iter = 4096;// number of iterations in PBKDF2 function
	int salt_size = 32;
	RAND_bytes(key, sizeof(key));
	//cout<<"Master key : "<<key<<endl;
	
	// form structure for writing on token
	const EVP_CIPHER * cipher_alg_mkey;
	cipher_alg_mkey = EVP_aes_256_ctr();// initialize structure for cipher algorithm
	token.MKeyCipherAlg = cipher_alg_mkey->nid;
	token.NumOfIterations = iter;
	const EVP_CIPHER * cipher_alg_data;
	cipher_alg_data = EVP_aes_256_ctr();// initialize structure for cipher algorithm
	token.DataCipherAlg = cipher_alg_data->nid;
	const EVP_MD *md = EVP_get_digestbyname("sha256");
	token.PrfFunction=md->type;
	do_hash_for_str(salt, salt_hash, token.PrfFunction);
	
	// build key for master key cipher
	PBKDF2_HMAC_SHA256(password, salt_hash, salt_size, iter, keylen, mk_cipher_key);
	cout<<"MKey key "<<mk_cipher_key<<endl;
	do_crypt_master_key(key, mkey_len,token.key, mk_cipher_key, token.MKeyCipherAlg);
	
	// derive information for token integrity control
	hmac_sha256(token.key, mkey_len, token.IntegrityCont, mk_cipher_key, token.PrfFunction);
	token.print("token.txt");
	//token.print("token.txt");
	//cout<<mk_cipher_key<<endl;
	//cout<<"Decrypt key : "<<key_dec<<endl;
	do_crypt_file(argv[1], key, token.DataCipherAlg);
	do_hash_for_file(argv[1],token.PrfFunction);
	
	return 0;
}