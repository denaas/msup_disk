#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
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
using namespace std;

#define BUFSIZE (4096)
#define BUFSIZE2 (32)
#define KEYLENGTH (32)
#define AUXKEYLENGTH (32)
#define HASHSIZE (32)
#define IVLENGTH (8)
#define ITERNUMBER (4096)

void PBKDF2_HMAC_SHA256(const char* password, const unsigned char* salt,
    int salt_size, int iterations, unsigned int keylen, unsigned char* out);
int do_crypt_file(const char *infile, unsigned char *key,unsigned char *iv, int nid);
int do_decrypt_file(const char *infile, unsigned char *key, unsigned char *iv, int nid);
int do_crypt_master_key(unsigned char *in, int inlen, unsigned char *out, 
	const unsigned char *key, unsigned char *iv, int nid);
int do_decrypt_master_key(unsigned char *in, int inlen, unsigned char *out, 
	unsigned char *key, unsigned char *iv, int nid);
void do_hash_for_str(const char*a, int inlen, unsigned char*mas,int nid);
void hmac_sha256(const unsigned char* in, int inlen, unsigned char* out, 
				unsigned char* key, int nid);
int do_hash_for_file(const char* infile,int id);
void check_hash_for_file(const char *infile,int id);
void make_token_file(const char *password, const char *salt,const char *file);
void encrypt(const char *password, const char *salt, const char *filename,const char*addrtoken);
void decrypt(const char *password, const char *salt, const char *filename,const char*addrtoken);


struct TokenStructure
{
// private:
	unsigned char *key;
	 //[KEYLENGTH];// ciphered key
	int MKeyCipherAlg;// object identificator of cipher 
								 //algorithm for master key
	unsigned int NumOfIterations;// number of iteration PBKDF2 function
	int DataCipherAlg;//object identificator of cipher
								 //algorithm for stored data
	int PrfFunction;//object identificator of hask-function
							   //used in PRF-function for making iteration keys
	unsigned char *IntegrityCont;
	 //[HASHSIZE];// integrity control information
 public:
 	TokenStructure()
 	{
 		key = new unsigned char [KEYLENGTH];
 		IntegrityCont = new unsigned char [HASHSIZE];
 		for (int i=0; i<KEYLENGTH; i++)
 			key[i]=0;
 		for (int i=0; i<HASHSIZE; i++)
 			IntegrityCont[i]=0;

 		MKeyCipherAlg=0;
 		NumOfIterations=0;
 		DataCipherAlg=0;
 		PrfFunction=0;
 	}
	~TokenStructure()
	{
		delete []key;
		delete []IntegrityCont;
	};
	void print(const char *);
	void read(const char *, const char *, const char *);
};

