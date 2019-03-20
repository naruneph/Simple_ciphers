#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <algorithm>
#include <cstring>
#include <cctype> 

using namespace std;

void improve_input(string &str){
	string::size_type pos = str.find('\r');
	if(pos!=string::npos){
		str.erase(str.begin()+pos,str.end());
	}
}

int get_mode(ifstream &f){
	string mode;
	getline(f,mode);
 	improve_input(mode);
	if (mode == "encrypt"){
		return 1;
	} else if(mode == "decrypt"){
		return 0;
	} else {
		throw 3;
	}
}

void ignore_space(ifstream &f){
	int c = f.peek();
	if(c != ' '){
		throw 4;
	}
	f.get();
}

string get_key(ifstream &f,unsigned int &len){
	string k;
	f >> len;
	ignore_space(f);
	getline(f,k);
	improve_input(k);
	if(len > k.length()){
		throw 4;
	} else if(len == 0 || k.length() == 0){
		throw 4;
		
	} else if(len < k.length()){
		k = k.substr(0,len);
	}
	for (unsigned int i=0; i<len; i++){
		if(!isalpha(k[i])){
			throw 4;
		} else {
			k[i] = tolower(k[i]);
		}
	}
	return k;
}

int count_tsize(ifstream &f){ 
	int pos = f.tellg();
	f.seekg(0,ios::end);
	int size = (int)f.tellg() - pos;
	f.seekg(pos);
	if(size == 0){
		throw 4;
	}
	return size;
}

int check_text(ifstream &f,int len){
	int pos = f.tellg();
	auto test = [](char c){return c == '\n' || c == '\r';};
	int inaccuracy =  count_if(istreambuf_iterator<char>(f),istreambuf_iterator<char>(),test);
	f.clear();
	f.seekg(pos);
	int size = count_tsize(f);
	int true_size = size - inaccuracy;

	if(true_size < len){
		throw 4;
	}
	return size;
}

string make_new_key(string k, int k_len,int t_len){
	string new_k = "";
	int int_part,fract_part;
	int_part = t_len / k_len;
	fract_part = t_len % k_len;
	for(int i=0; i < int_part; i++){
		new_k+=k;
	}
	new_k+=k.substr(0,fract_part);
	return new_k;
}

void encode(ifstream &f,string k,int full_len,int t_len){
	char c;
	int j=0;
	int i,t;
	for(i=0,t=0; (i<full_len) && (t<t_len); i++){
		f.get(c);
		if(!isalpha(c)){
			cout << c;
			if(!iscntrl(c)){
				t++;
			}
		} else {
			if(isupper(c)){
				c = ((((c-'A') + (toupper(k[j])-'A')) % 26) + 'A');
			} else {
				c = ((((c-'a') + (k[j]-'a')) % 26) + 'a');
			}
			cout << c;
			j++;
			t++;
		}
	}
}

void decode(ifstream &f,string k,int full_len,int t_len){
	char c;
	int j=0;
	int i,t;
	for(i=0,t=0; (i<full_len) && (t<t_len); i++){
		f.get(c);
		if(!isalpha(c)){
			cout << c;
			if(!iscntrl(c)){
				t++;
			}
		} else {
			if(isupper(c)){
				c = ((((c-'A') - (toupper(k[j])-'A') +26) % 26) + 'A');
			} else {
				c = ((((c-'a') - (k[j]-'a') +26 ) % 26) + 'a');
			}
			cout << c;
			j++;
			t++;
		}
	}	

}

int main(int argc, char** argv){
	try{
		if(argc != 2){
			throw 1;
		}
		ifstream file(argv[1]);
		if(!file.is_open()){
			throw 2;
		}
		int mode = get_mode(file);
		unsigned int key_length,text_length;	 
		string key = get_key(file, key_length);

		file >> text_length;
		ignore_space(file);

		int full_length = check_text(file,text_length);

		if(text_length == 0){
		 	cout<<""<<endl;
		 	return 0;
		}
		
		string new_key = make_new_key(key,key_length,text_length);
		if(mode){
			encode(file,new_key,full_length,text_length);
		} else {
			decode(file,new_key,full_length,text_length);
		}
	}
	catch(int i){
		switch (i){
			case 1: cerr<<"NO FILE"<<endl; break;
			case 2: cerr<<"CANNOT OPEN FILE"<<endl; break;
			case 3: cerr<<"UNKNOWN MODE"<<endl; break;
			case 4: cerr<<"WRONG INPUT"<<endl; break;
			default: cerr<<"ERROR"<<endl;
		}		
	}
	return 0;
}
