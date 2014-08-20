#ifndef RSA_H
#define RSA_H 1

#include "integer.hpp"
#include <string>

#define RSA_KEY_BYTES 64
//#define RSA_KEY_BYTES 128

//number input+output format:
//a string of unsigned chars, each representing 0-255
//the most segnificant bit is in the first element [0]
//so [FF,00] is 65025, [FF,01] is 65026

int rsa_verify(unsigned char** result,unsigned char* message, unsigned char* public_key, unsigned char* modulus, unsigned int keyLengthBytes);
int rsa_sign(unsigned char** result, unsigned char* hash, unsigned int hashLengthBytes, unsigned char* privateKey, unsigned char* modulus, unsigned int keyLengthBytes);
std::string rsa_sign_file(const char* file);
std::string rsa_sign_file(const char* file,const char* keyfile);
std::string rsa_verify_file(const char* file);

integer hex_to_integer(unsigned char* data, unsigned int datalen, unsigned int pad);
std::string dechex(unsigned char i);
unsigned char hexdec(std::string inputString);

#endif
