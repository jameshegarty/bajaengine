#include "Helperlib/Containers.hpp"

#include "md5.h"
#include "rsa.h"
#include "random.h"


#include "integer/integer.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include "engine_security.h"
#include <sstream>
#include <new>
 
void rsaerror(char* error){

}

integer hex_to_integer(unsigned char* data, unsigned int datalenBytes, unsigned int pad){

	integer base=string_to_integer("256");

	int siz=datalenBytes;

	integer radix=create_integer(siz);
	set_zero_integer(radix);
	radix.c[0]=1;
	
	integer result=create_integer(siz*2+pad);
	set_zero_integer(result);

	integer digit=create_integer(1);

	for(int i=datalenBytes-1; i>=0; i--){
		digit.c[0]=data[i];
		integer part_res=create_integer(1+radix.num_components+1);
		set_zero_integer(part_res);

		multiply_integer(digit,radix,part_res);

		add_integer(result,part_res,result);
		free_integer(part_res);

		integer radix_temp=create_integer(radix.num_components);
		copy_integer(radix,radix_temp);

		multiply_integer(radix_temp,base,radix);
		free_integer(radix_temp);
	}

	optimize_integer(result);

	return result;
}

unsigned int integer_to_binary(unsigned char** result,integer data,int keyLenBytes){
	integer base=string_to_integer("256");

	integer div=create_integer(data.num_components);
	copy_integer(data,div);

	integer zero=create_integer(1);
	zero.c[0]=0;


	unsigned char* pf=new unsigned char[data.num_components];

	for(int i=0; i<data.num_components; i++){
		pf[i]=0;
	}

	int tpos=data.num_components-1;

	pf[tpos]=div.c[0];
	tpos--;

	while(compare_integers(div,zero)==1){
		integer mod=create_integer(div.num_components+1+1);
		mod_integer(div,base,mod);

		integer div_temp=create_integer(div.num_components);
		copy_integer(div,div_temp);
		divide_small_integer(div_temp,256,div);

		pf[tpos]=div.c[0];
		tpos--;

	}

	int startpos=div.num_components-keyLenBytes;

	*result=new unsigned char[keyLenBytes];

	for(int i=0; i<keyLenBytes; i++){
		(*result)[i]=pf[i+startpos];
	}

	return keyLenBytes;
}

void add_PKCS1_padding(unsigned char* padded, unsigned char* data, unsigned int datalenBytes, bool isPublicKey, unsigned int blocksizeBytes)
{
	//the '3' is the 3 metadata bytes we have to add
	unsigned int pad_length = blocksizeBytes - 3 - datalenBytes;

	unsigned char block_type;
	unsigned char* padding=new unsigned char[pad_length];

	if(isPublicKey)
	{
		block_type = 0x02;

		for(unsigned int i = 0; i < pad_length; i++)
		{
			unsigned char rnd=randomGen.IRandom(1,255);	//NOTE:  this can't be 0!!! this will cause the unpad to jump out early

			padding[i]=rnd;
		}
	}
	else
	{
		block_type =  0x01;

		for(unsigned int i = 0; i < pad_length; i++)
		{
			padding[i]=0;
		}
	}
	
	padded[0]=0x00;
	padded[1]=block_type;

	for(unsigned int i=0; i<pad_length; i++){
		padded[i+2]=padding[i];
	}
	padded[2+pad_length]=0x00;	//5

	for(unsigned int i=0; i<datalenBytes; i++){
		padded[3+pad_length+i]=data[i];
	}

}

int remove_PKCS1_padding(unsigned char** result,unsigned char* data,unsigned int keyLengthBytes){

	if(data[1] == 0x00 ||  (data[1] != 0x01 && data[1] != 0x02)){
		rsaerror("Block type not implemented.");
	}

	unsigned int startpos=0;

	for(unsigned int i=1; i<keyLengthBytes; i++){
		if(data[i]==0x00){
			startpos=i;
		}
	}

	*result=new unsigned char[keyLengthBytes-startpos];

	for(unsigned int i=startpos; i<keyLengthBytes; i++){
		(*result)[i-startpos]=data[i+1];
	}

	return keyLengthBytes-startpos-1;
}

void newh(){
	std::cout << "OUT OF RAM" << std::endl;
	exit(1);
}

int pow_mod(unsigned char** result,unsigned char* p,unsigned  char* q, unsigned char* r,unsigned int keyLengthBytes){

	std::set_new_handler(newh);

	Array<integer> factors;

	integer div=hex_to_integer(q,keyLengthBytes,0);

	int siz=keyLengthBytes;

	integer power_of_two=create_integer(siz);
	set_zero_integer(power_of_two);

	integer zero=create_integer(1);
	set_zero_integer(zero);

	integer two=string_to_integer("2");
	integer one=string_to_integer("1");

	int i=0;
	integer div_temp=create_integer(div.num_components);
	integer rem=create_integer(div.num_components+2);
	while(compare_integers(div, zero) == 1){
		set_zero_integer(rem);
		mod_integer(div, two,rem);

		copy_integer(div,div_temp);
		divide_small_integer(div_temp,2,div);
		
		if(compare_integers(rem, zero) == 1){
			integer ptt=create_integer(power_of_two.num_components);
			copy_integer(power_of_two,ptt);
			factors.pushBack(ptt);
		}
		
		add_integer(power_of_two,one,power_of_two);
		
		i++;
	}
	free_integer(rem);
	free_integer(div_temp);

	Array<integer> partial_results;

	integer part_res=hex_to_integer(p,keyLengthBytes,2);
	integer ri=hex_to_integer(r,keyLengthBytes,1);
	integer idx = create_integer(part_res.num_components);
	
	set_zero_integer(idx);

	integer part_res_temp1=create_integer(part_res.num_components);
	integer part_res_temp2=create_integer(part_res.num_components);
	integer part_res_temp3=create_integer(part_res.num_components);

	for(int i=0; i<factors.size(); i++){

		while(compare_integers(factors[i],idx)==1){
			copy_integer(part_res,part_res_temp1);
			copy_integer(part_res,part_res_temp2);

			multiply_integer(part_res_temp1,part_res_temp2,part_res);
			
			copy_integer(part_res,part_res_temp3);

			mod_integer(part_res_temp3,ri,part_res);

			add_integer(idx,one,idx);
		}

		integer ptt=create_integer(part_res.num_components);
		copy_integer(part_res,ptt);
		partial_results.pushBack(ptt);
	}

	free_integer(part_res_temp1);
	free_integer(part_res_temp2);
	free_integer(part_res_temp3);

	integer resulti=create_integer(ri.num_components+1);
	set_zero_integer(resulti);
	resulti.c[0]=1;

	integer resulti_temp=create_integer(resulti.num_components);
	integer resulti_temp2=create_integer(resulti.num_components);
	for(int i=0; i<partial_results.size(); i++){
		
		copy_integer(resulti,resulti_temp);

		multiply_integer(resulti_temp,partial_results[i],resulti);

		copy_integer(resulti,resulti_temp2);

		mod_integer(resulti_temp2,ri,resulti);
	}


	free_integer(resulti_temp);
	free_integer(resulti_temp2);

	int lol=integer_to_binary(result,resulti,keyLengthBytes);

	return lol;

}

int rsa_decrypt(unsigned char** result,unsigned char* message,unsigned char* private_key, unsigned char* modulus, unsigned int keyLengthBytes){
	unsigned char* tresult;
	
	int size=remove_PKCS1_padding(result, tresult,keyLengthBytes);

	return size;
}

int rsa_sign(unsigned char** result, unsigned char* hash, unsigned int hashLengthBytes, unsigned char* privateKey, unsigned char* modulus, unsigned int keyLengthBytes){
	
	if(hashLengthBytes>(keyLengthBytes-11)){
		return 0;
	}

	unsigned char* padded=new unsigned char[keyLengthBytes];
	add_PKCS1_padding(padded,hash,hashLengthBytes,false,keyLengthBytes);

	int r=pow_mod(result,padded,privateKey,modulus,keyLengthBytes);

	return r;
}


int rsa_verify(unsigned char** result,unsigned char* message, unsigned char* public_key, unsigned char* modulus, unsigned int keyLengthBytes){
	return rsa_decrypt(result, message, public_key, modulus, keyLengthBytes);
}


char* hexNumber=NULL;

std::string dechex(unsigned char i){

	if(hexNumber==NULL){
		hexNumber=new char[2];
	}

	sprintf(hexNumber,"%x",i);
	
	std::string r(hexNumber);

	if(r.size()==1){
		r="0"+r;
	}

	return r;
}

unsigned char hexdec(std::string inputString){
	unsigned char r=0;

	if(inputString[1]=='0'){r=0;
	}else if(inputString[1]=='1'){r=1;
	}else if(inputString[1]=='2'){r=2;
	}else if(inputString[1]=='3'){r=3;
	}else if(inputString[1]=='4'){r=4;
	}else if(inputString[1]=='5'){r=5;
	}else if(inputString[1]=='6'){r=6;
	}else if(inputString[1]=='7'){r=7;
	}else if(inputString[1]=='8'){r=8;
	}else if(inputString[1]=='9'){r=9;
	}else if(inputString[1]=='a'){r=10;
	}else if(inputString[1]=='b'){r=11;
	}else if(inputString[1]=='c'){r=12;
	}else if(inputString[1]=='d'){r=13;
	}else if(inputString[1]=='e'){r=14;
	}else if(inputString[1]=='f'){r=15;
	}

	if(inputString[0]=='0'){r+=0;
	}else if(inputString[0]=='1'){r+=16;
	}else if(inputString[0]=='2'){r+=32;
	}else if(inputString[0]=='3'){r+=48;
	}else if(inputString[0]=='4'){r+=64;
	}else if(inputString[0]=='5'){r+=80;
	}else if(inputString[0]=='6'){r+=96;
	}else if(inputString[0]=='7'){r+=112;
	}else if(inputString[0]=='8'){r+=128;
	}else if(inputString[0]=='9'){r+=144;
	}else if(inputString[0]=='a'){r+=160;
	}else if(inputString[0]=='b'){r+=176;
	}else if(inputString[0]=='c'){r+=192;
	}else if(inputString[0]=='d'){r+=208;
	}else if(inputString[0]=='e'){r+=224;
	}else if(inputString[0]=='f'){r+=240;
	}

	return r;
}

bool keyread=false;
unsigned char file_public_modulus[RSA_KEY_BYTES];
unsigned char file_private_exponent[RSA_KEY_BYTES];

std::string readkey(const char* keyfile){
	if(keyread){return "";}
	keyread=true;

	std::ifstream keyin(keyfile); // Open for reading

	if(!keyin.is_open()){
		
		return "key.lmk not found";
	}

	Array<std::string> f;
	std::string st;
	while(getline(keyin,st)){
		f.pushBack(st);
	}

	

	for(int i=0; i<RSA_KEY_BYTES; i++){
		std::string t=f[0].substr(i*3,2);
		file_public_modulus[i]=hexdec(t);
	}

	for(int i=0; i<RSA_KEY_BYTES; i++){
		std::string t=f[1].substr(i*3,2);
		file_private_exponent[i]=hexdec(t);
	}

	keyin.close();

	return "";
}

std::string rsa_sign_file(const char* file,const char* keyfile){

	std::string lol=readkey(keyfile);
	if(lol!=""){return lol;}

	std::ifstream in (file,std::ios::in|std::ios::binary);

	in.sync();

	in.seekg(256+4,std::ios_base::beg);

	unsigned int hashlen=102400;

	in.seekg(0,std::ios::end);
	
	unsigned int totallen=in.tellg();

	totallen-=260;
	if(totallen<hashlen){
		hashlen=totallen;
	}

	in.seekg(256+4,std::ios_base::beg);

	unsigned char* hashmaker=new unsigned char[hashlen];
	in.read((char*)hashmaker,hashlen);

	in.close();

	//////////////////////////////////////

	MD5 md5checksumB(hashmaker,hashlen);
	unsigned char* hexx=new unsigned char[33];
	hexx=(unsigned char*)md5checksumB.hex_digest();

	
	/////////////////////////////////
	unsigned char* resulth;
	int size_s=0;
	size_s=rsa_sign(&resulth,hexx,32,file_private_exponent,file_public_modulus,RSA_KEY_BYTES);

	if(size_s!=RSA_KEY_BYTES){

		return "Sig Length Error";
	}

	std::ofstream out;
	out.open(file, std::ios::in|std::ios::out|std::ios::binary);

	out.seekp(4,std::ios_base::beg);
	out.write((char*)resulth,RSA_KEY_BYTES);

	
	
	out.close();

	delete[] resulth;

	delete[] hashmaker;
	delete[] hexx;

	return "";
}

std::string rsa_sign_file(const char* file){
	return rsa_sign_file(file,"key.lmk");
}

#ifndef RSA
unsigned char release_modulus[RSA_KEY_BYTES];
unsigned char user_public_modulus[RSA_KEY_BYTES];

std::string rsa_verify_file(const char* file){

	std::ifstream in(file,std::ios::in|std::ios::binary);

	in.seekg(4,std::ios::beg);

	unsigned char* encrypted=new unsigned char[RSA_KEY_BYTES];
	in.read((char*)encrypted,RSA_KEY_BYTES);

	unsigned int hashlen=102400;

	in.seekg(0,std::ios::end);
	unsigned int totallen=int(in.tellg())-260;
	if(totallen<hashlen){
		hashlen=totallen;
	}

	in.seekg(256+4,std::ios_base::beg);

	unsigned char* hashmake=new unsigned char[hashlen];
	in.read((char*)hashmake,hashlen);


	MD5 md5checksumB(hashmake,hashlen);
	unsigned char* hexx=new unsigned char[33];
	hexx=(unsigned char*)md5checksumB.hex_digest();

	in.close();

	unsigned char* resulth;

	unsigned char public_exponent[RSA_KEY_BYTES];	//[]={'0xFF','0xFF','0x01'};
	for(int i=0; i<(RSA_KEY_BYTES-3); i++){public_exponent[i]=0;}
	public_exponent[RSA_KEY_BYTES-3]=0x01;	//65536
	public_exponent[RSA_KEY_BYTES-2]=0x00;	//0
	public_exponent[RSA_KEY_BYTES-1]=0x01;	//1

	int size_s=rsa_verify(&resulth,encrypted,public_exponent,user_public_modulus,RSA_KEY_BYTES);

	if(size_s!=32){
		std::ostringstream tos;
		tos << size_s;

		return "invalid key";
	}

	for(int i=0; i<32; i++){
		if(hexx[i]!=resulth[i]){
			std::ostringstream t;
			t << i;

			return "invalid key";
		}
	}

	delete[] encrypted;
	delete[] hashmake;
	delete[] hexx;

	return "";
}
#endif
