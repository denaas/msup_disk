#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
using namespace std;

struct TokenStructure
{
// private:
	unsigned char key[32];// ciphered key
	int MKeyCipherAlg;// object identificator of cipher 
								 //algorithm for master key
	unsigned int NumOfIterations;// number of iteration PBKDF2 function
	int DataCipherAlg;//object identificator of cipher
								 //algorithm for stored data
	int PrfFunction;//object identificator of hask-function
							   //used in PRF-function for making iteration keys
	unsigned char IntegrityCont[32];// integrity control information
 public:
	~TokenStructure(){};
	void print(const char *);
};
