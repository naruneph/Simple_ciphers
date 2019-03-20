#include <iostream>
#include <fstream>
#include <string> 
#include <map>

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

string get_key(ifstream &f){
	string k;
	getline(f,k);
	improve_input(k);
	for (unsigned int i=0; i<k.length(); i++){
		if(!isalpha(k[i])){
			throw 4;
		} else {
			k[i] = tolower(k[i]);
			if(k[i] == 'j'){
				k[i] = 'i';
			}
		}
	}
	return k;	
}

char** make_matrix(string &k, map<char,pair<int,int>> &m){
	string alphabet("abcdefghiklmnopqrstuvwxyz");
	char** em = (char**) malloc(5*sizeof(char*));
	for(int i=0; i<5; i++){
		em[i] =(char*) malloc(5*sizeof(char));
	}
	int i=0;
	for(char &c: k){
		if(m.find(c) == m.end()){
			m[c] = make_pair(i/5, i%5);
			em[i/5][i%5] = c;
			i++;
		}
	}
	for(char &c: alphabet){
		if(m.find(c) == m.end()){
			m[c] = make_pair(i/5, i%5);
			em[i/5][i%5] = c;
			i++;
		}
	}
	return em;	
}

string make_digram(ifstream &f){
	string dgrm="";
	char c;
	while(f.get(c) && !isalpha(c)){
		if(ispunct(c) || isspace(c)){
			cout << c;
		}
		else{
			throw 4;
		}
	}
	if(!f.eof()){
		if(c == 'j' || c == 'J'){
			c--;
		} 
		dgrm+=c;
	}
	while(f.get(c) && !isalpha(c)){
		if(ispunct(c) || isspace(c)){
			dgrm+=c;
		}
		else{
			throw 4;
		}
	}

	if(!f.eof()){
		if(c == 'j' || c == 'J'){
			c--;
		}		
		if((dgrm[0] == tolower(c)) || (dgrm[0] == toupper(c)) ){
			f.unget();
			dgrm+='X';
		} else {
	 		dgrm+=c;
	 	}
	}
	if(dgrm.length() != 0 && (dgrm.length() == 1 || !isalpha(dgrm[dgrm.length()-1] ))){
		dgrm+='X';
	}
	if(dgrm.length() > 2){
		
		char x = dgrm[dgrm.length()-1];;
	 	string tmp(1,x);
		dgrm.insert(1,tmp);
		dgrm.pop_back();
	}

	return dgrm; 
}

void run_the_algorithm(ifstream &f, map<char,pair<int,int>> &m, char** em, int n){
	string digram;
	int flag = 0;
	while( (digram = make_digram(f)) != ""){
		flag = 1;
		int i1 = m[tolower(digram[0])].first;
		int j1 = m[tolower(digram[0])].second;
		int i2 = m[tolower(digram[1])].first;
		int j2 = m[tolower(digram[1])].second;
		if(i1 == i2){//same row
			j1 = (j1+n) % 5;
			j2 = (j2+n) % 5;
		} else if(j1 == j2){//same column
			i1 = (i1+n) % 5;
			i2 = (i2+n) % 5;
		} else {//different row and column
			swap(j1,j2);
		}
		if(isupper(digram[0])){
			digram[0] = toupper(em[i1][j1]);
		} else {
			digram[0] = em[i1][j1];
		}
		if(isupper(digram[1])){
			digram[1] = toupper(em[i2][j2]);
		} else {
			digram[1] = em[i2][j2];
		}
		cout<<digram;
	}
	if (flag == 0){
		throw 4;
	}
	cout<<endl;
}

void free_matrix(char** m, int r){
	for(int i=0; i<r; i++){
		free(m[i]);
	}
	free(m);	
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
		string key = get_key(file); 
		map<char,pair<int,int>> matrix;
		char** extra_matrix = make_matrix(key, matrix);
		if(mode){
			run_the_algorithm(file,matrix,extra_matrix,1);
		} else {
			run_the_algorithm(file,matrix,extra_matrix,4);
		}
		free_matrix(extra_matrix, 5);
		file.close();
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