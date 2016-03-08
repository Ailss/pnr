#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <map>
#include <fstream>
#include <bitset>

#ifndef TYPEH
#define TYPEH
#include "type.h"
#endif


using namespace std;

struct vertex
{
	string name;
	int type;
	vector<int> in_v;
	vector<int> out_v;
	bitset<16> sreg;
	
	vertex()
	{
		in_v.clear();
		out_v.clear();
		type=0;

	}
};

class parser
{
	public:
		
		vector<vertex> vtex;
		int out_source[OUT_PIN+1];
		string model;
		map<string,int> pin_map;
		map<int,int> pin_v;
		int v_count;
		int pin_count;
		int in_pin,out_pin;
		
		void read_blif();
		parser(char*);
		~parser();
		
	private:
		ifstream in_blif;
		
		
		void deal_outputs(char* outputs);
		void deal_inputs(char* inputs);
		void deal_gate(char* gate);
		void help_lut4_input(char *buf,vertex &v);
		void help_lut4_output(char *buf,vertex &v);

		void deal_name(char* name);
};


extern int is_input(int);
extern int is_output(int);







