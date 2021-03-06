
#include "types.hpp"
using namespace std;
 

void TokenStructure::print(const char *file)
{
	FILE *in;
	if(!(in=fopen(file,"w+"))) cout<<"Error open for write file "<<file<<endl;
	fwrite(key, 1, KEYLENGTH, in);
	//fwrite(key, 1, 32, cout);
	fprintf(in," %d %d %d %d ", MKeyCipherAlg, NumOfIterations, DataCipherAlg,
		PrfFunction);
	fwrite(IntegrityCont,1, HASHSIZE, in);
}

void TokenStructure::read(const char *file)//функция считывания из файла
{
	FILE *in;
	int *tmp=new int[1];
	if(!(in=fopen(file,"rb+"))) cout<<"Error open file "<<file<<endl;
	fread(key, 1, 32, in);
	//cout<<"Read key: "<<key<<endl;
	fread(tmp,1,1,in);
	MKeyCipherAlg=tmp[0];
	fread(tmp,1,1,in);
	NumOfIterations=tmp[0];
	fread(tmp,1,1,in);
	DataCipherAlg=tmp[0];
	fread(tmp,1,1,in);
	PrfFunction=tmp[0];
	fread(IntegrityCont,1, 32, in);
	delete []tmp;
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
 
int do_crypt_file(char *infile, unsigned char *key,unsigned char *iv, int nid)
{
	const char *outfile="tmp";//temporary file for ciphered data
	int outlen, inlen;
	FILE *in, *out;
	unsigned char inbuf[BUFSIZE], outbuf[BUFSIZE];
	EVP_CIPHER_CTX ctx;// контекст алгоритма шифрования
	const EVP_CIPHER * cipher;
	
	//open files
	if(!(in=fopen(infile,"rb"))) cout<<"Error open file "<<infile<<endl;
	if(!(out=fopen(outfile,"wb"))) cout<<"Error open file "<<outfile<<endl;
 
	// Обнуляем структуру контекста 
	EVP_CIPHER_CTX_init(&ctx);
 
	// Выбираем алгоритм шифрования 
	cipher = EVP_get_cipherbynid(nid);
 
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

int do_decrypt_file(char *infile, unsigned char *key, unsigned char *iv, int nid)
{
	// Объявляем переменные 
	int outlen, inlen;// length of read and written data
	FILE *in, *out;
	const char *outfile="tmp.txt";
	unsigned char inbuf[BUFSIZE], outbuf[BUFSIZE];
	EVP_CIPHER_CTX ctx;// context structure
	const EVP_CIPHER * cipher;// structure for cipher algorithm
 
	// Обнуляем контекст и выбираем алгоритм дешифрования 
	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_get_cipherbynid(nid);// initialize structure for cipher algorithm
	EVP_DecryptInit(&ctx, cipher, key, iv);
 
	// Открываем входной и создаем выходной файлы 
	if(!(in=fopen(infile,"rb"))) cout<<"Error open file "<<infile<<endl;
	if(!(out=fopen(outfile,"wb"))) cout<<"Error open file "<<outfile<<endl;
	
	// Расшифровываем данные 
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

int do_crypt_master_key(unsigned char *in, int inlen, unsigned char *out, 
	const unsigned char *key, unsigned char *iv, int nid)
{
	int outlen=0; 
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

int do_decrypt_master_key(unsigned char *in, int inlen, unsigned char *out, 
	unsigned char *key, unsigned char *iv, int nid)
{
	//cout<<"Crypt key in Decrypt master key: "<<in<<endl;
	// Объявляем переменные 
	int outlen;// length of read and written data
	EVP_CIPHER_CTX ctx;// context structure
	const EVP_CIPHER * cipher;// structure for cipher algorithm

	// Обнуляем контекст и выбираем алгоритм дешифрования 
	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_get_cipherbynid(nid);// initialize structure for cipher algorithm
	EVP_DecryptInit(&ctx, cipher, key, iv);
	// Расшифровываем данные 
	if(!EVP_DecryptUpdate(&ctx, out, &outlen, in, inlen)) return 0;
	// Завершаем процесс дешифрования, дешифруем оставшиеся биты
	if(!EVP_DecryptFinal(&ctx, out, &outlen)) return 0;
	EVP_CIPHER_CTX_cleanup(&ctx);// clean structure of cipher context
}

void do_hash_for_str(const char*a, int inlen, unsigned char*mas,int nid)//функция,создающая хеш для строки
{
	
	EVP_MD_CTX mdctx; // контекст для вычисления хэша 
	const EVP_MD * md; // структура с адресами функций алгоритма 
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len; // размер вычисленного хэша 
	md = EVP_get_digestbynid(nid);// Получаем адреса функций алгоритма SHA256 и инициализируем контекст для вычисления хэша 
	
	// Вычисляем хэш 
		EVP_DigestInit(&mdctx, md);
		EVP_DigestUpdate(&mdctx, a, inlen);
		EVP_DigestFinal(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);

		for(int i=0; i<BUFSIZE2; ++i)
			mas[i]=md_value[i];	
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
	tmp = fopen("tmp.txt","w+");
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
		fwrite(buf,1,strlen((const char *)buf),tmp);
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
	fclose(tmp);
	remove(infile);
	rename(tmp,infile);
	return 1;

}

void hmac_sha256(const unsigned char* in, int inlen, unsigned char* out, 
				unsigned char* key, int nid)
{
    unsigned int len = HASHSIZE;
 	//unsigned char* result;
    //result = (unsigned char*)malloc(sizeof(char) * len);
    const EVP_MD * md;
 	md = EVP_get_digestbynid(nid);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);// clear structure
    // Using sha256 hash engine here.
    HMAC_Init(&ctx, key, KEYLENGTH, md);
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

void make_token_file(const char *password, const char *salt, 
	TokenStructure &token, char *file)
{
	unsigned char key[KEYLENGTH]; // 256 bits master key 
	unsigned char key_dec[KEYLENGTH];// decrypt aster key
	int mkey_len=KEYLENGTH;// master key length
	int salt_len = strlen(salt);
	unsigned char mk_cipher_key[AUXKEYLENGTH];//256 bits key for cipher master key
	unsigned char salt_hash[HASHSIZE];
	unsigned char iv_mkey[IVLENGTH];// initialization vector for m-key crypt
	unsigned char iv_data[IVLENGTH];// initialization vector for data crypt
	int keylen = AUXKEYLENGTH;// length of key for master key cipher
	int iter = ITERNUMBER;// number of iterations in PBKDF2 function
	//int salt_size = 32;
	RAND_bytes(key, sizeof(key));
	RAND_bytes(iv_mkey, sizeof(iv_mkey));
	RAND_bytes(iv_data, sizeof(iv_data));
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
	do_hash_for_str(salt, salt_len, salt_hash, token.PrfFunction);
	// build key for master key cipher
	PBKDF2_HMAC_SHA256(password, salt_hash, HASHSIZE, iter, keylen, mk_cipher_key);
	//cout<<"MKey key "<<mk_cipher_key<<endl;
	do_crypt_master_key(key, mkey_len,token.key, mk_cipher_key, 
		iv_mkey, token.MKeyCipherAlg);
	//cout<<"Ciphered key: "<<token.key<<endl;
	// derive information for token integrity control
	hmac_sha256(token.key, mkey_len, token.IntegrityCont, mk_cipher_key, token.PrfFunction);
	// write to token file
	token.print("token.txt");
	//cout<<mk_cipher_key<<endl;
	do_decrypt_master_key(token.key, mkey_len, key_dec, mk_cipher_key, 
		iv_mkey, token.MKeyCipherAlg);
	//cout<<"Decrypt key : "<<key_dec<<endl;
	if (!memcmp(key, key_dec, KEYLENGTH)) 
	{
		cout<<"YES\n"; 
		do_crypt_file(file, key, iv_data, token.DataCipherAlg);	
		do_decrypt_file(file, key_dec, iv_data, token.DataCipherAlg);
	}
	else cout<<"NO\n";
}

void read_from_token(const char *password, const char *salt, TokenStructure &token, 
	const char *file)
{
	unsigned char integrityCont[HASHSIZE];
	unsigned char mk_cipher_key[AUXKEYLENGTH];
	int mkey_len=KEYLENGTH;// master key length
	int salt_len = strlen(salt);
	unsigned char salt_hash[HASHSIZE];
	int keylen = AUXKEYLENGTH;// length of key for master key cipher
	int iter = ITERNUMBER;// number of iterations in PBKDF2 function
	FILE *in;
	if(!(in=fopen(file,"rb+"))) cout<<"Error open for read file "<<file<<endl;
	cout<<"OK1\n";
	// read from file
	fread(token.key, 1, KEYLENGTH, in);
	cout<<"OK2\n";
	fscanf(in," %d %d %d %d ", token.MKeyCipherAlg, token.NumOfIterations, 
		token.DataCipherAlg, token.PrfFunction);
	cout<<"OK2\n";
	fread(token.IntegrityCont,1, HASHSIZE, in);
	//cout<<token.key<<endl;
	//printf(" %d %d %d %d ", token.MKeyCipherAlg, token.NumOfIterations, 
	//	token.DataCipherAlg, token.PrfFunction);
	cout<<"OK4 \n";

	// derive information for integrity control
	do_hash_for_str(salt, salt_len, salt_hash, token.PrfFunction);
	cout<<"OK5\n";
	// build key for master key cipher
	PBKDF2_HMAC_SHA256(password, salt_hash, HASHSIZE, iter, keylen, mk_cipher_key);
	cout<<"OK6\n";
	hmac_sha256(token.key, KEYLENGTH, integrityCont, mk_cipher_key, token.PrfFunction);
	cout<<"OK7\n";
	if (!memcmp(integrityCont, token.IntegrityCont, HASHSIZE)) 
		cout<<"INTEGRITY CONTROL SUCCEDED\n"; 
	else cout<<"INTEGRITY CONTROL FAILED !\n";
}

int main(int argc, char** argv)
{
	if (argc!=2)
	{
		cout<<"Usage: "<<argv[0]<<" <in_file.txt> "<<endl;
		return 0;
	}
	OpenSSL_add_all_algorithms();// load information about all cipher algorithms
	TokenStructure token, token_read;
	const char* password = "password";
	const char *salt = "salt";
	
	make_token_file(password, salt, token, argv[1]);
	//read_from_token(password, salt, token_read, "token.txt");
	//token_read.read("token.txt");
	//cout<<token_read.MKeyCipherAlg<<" "<<token_read.NumOfIterations<<" "<<
	//token_read.DataCipherAlg<<" "<<token_read.PrfFunction<<endl;
	return 0;
}

