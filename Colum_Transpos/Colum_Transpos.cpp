#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <algorithm>
#include <cstring>
#include <cmath>
#include <vector>

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
		throw 5;
	}
}

vector<int> get_key (ifstream &f){
	vector<int> vect;
	string str,substr;
	getline(f,str);
	improve_input(str);
	stringstream ss(str);
	while(ss.good()){       
        getline(ss, substr, ',');

        if(substr == ""){      
        	throw 1;
        } else {        	
 	       for(char &c: substr){
        		if(!isdigit(c)){
        			throw 1;
        		}
        	}
        }

        vect.push_back( atoi(substr.c_str()) );
    }
	for (unsigned int i=1; i<=vect.size(); i++){ 
		if(find(vect.begin(), vect.end(), i) == vect.end()){
			throw 1;
		}
	}
    return vect;
}

vector<int> make_new_key(vector<int> k,int flag){
	vector<int> new_k(k.size());
	for(unsigned int i=0; i<k.size(); i++){
	    k[i]--;
	    new_k[k[i]]=i; 
	}
	if(flag){
		return new_k;
	} else {
		return k;
	}			
}

int count_tsize(ifstream &f){ 
	int pos = f.tellg();	
	f.seekg(0,ios::end);
	int size = (int)f.tellg() - pos;
	f.seekg(pos);
	if(size == 0){
		throw 2;
	}
	return size;
}

void get_matrix_size(ifstream &f, int &r, int &c, vector<int> k,int flag){
	int text_size = count_tsize(f);

	int pos = f.tellg();
	auto test = [](char c){return c == '\n' || c == '\r';};
	int inaccuracy =  count_if(istreambuf_iterator<char>(f),istreambuf_iterator<char>(),test);
	f.clear();
	f.seekg(pos);

	int true_size = text_size - inaccuracy;

	c = k.size();
	r = true_size / (c*sizeof(char));
	if(true_size % c != 0){
	    r++;
	}	
	if(!flag){
		swap(r,c);
	}
}

char** make_matrix(ifstream &f, int r, int c){
	char** mtrx = (char**) malloc(r*sizeof(char*));
	for(int i=0; i<r; i++){
		mtrx[i] =(char*) malloc(c*sizeof(char));
	}
	char* buf = (char*) malloc(c*sizeof(char));	
	int cnt;
	char ch;
	for(int i=0; i<r; i++){
		memset(buf,' ',c*sizeof(char));
		cnt = 0;
		while((cnt<c) && (f.get(ch))){
			if((ch!='\n') && (ch!='\r')){
				buf[cnt] = ch;
				cnt++;
			}
		}
		memcpy(mtrx[i], buf,c*sizeof(char));
	}
	free(buf);
	return mtrx;	
}

void run_the_algorithm(char** m,int r, int c, vector<int> k,int flag){
	for(int i=0; i<c; i++){
		for(int j=0;j<r; j++){
			if(flag){
	    		cout<<m[j][k[i]];
	    	} else {
	    		cout<<m[k[j]][i];
	    	}
		}
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
	    if (argc != 2){
	    	throw 3;
	    }
		ifstream file(argv[1]);
		if (!file.is_open()){
			throw 4;
		}
	    int mode = get_mode(file);	
		vector<int> key = get_key(file);
		vector<int> new_key = make_new_key(key,mode);

		int row,col;

		get_matrix_size(file,row,col,new_key,mode);
		char** matrix = make_matrix(file,row,col);
	    run_the_algorithm(matrix,row,col,new_key,mode);
	    free_matrix(matrix,row);
		
	    file.close();
	}
	catch(int i){
		switch (i){
			case 1: cerr<<"INVALID KEY"<<endl; break;
			case 2: cerr<<"NO TEXT"<<endl; break;
			case 3: cerr<<"NO FILE"<<endl; break;
			case 4: cerr<<"CANNOT OPEN FILE"<<endl; break;
			case 5: cerr<<"UNKNOWN MODE"<<endl; break;
			default: cerr<<"ERROR"<<endl;
		}
	}
    return 0;
}