#ifndef PARSER_H
#define PARSER_H
#include "parser.h"
#endif


using namespace std;

#ifndef LOGIC_CELL
#define LOGIC_CELL 240

#endif



class place
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
		int groups[4];
		
		vertex cells[LOGIC_CELL+1];
		
		place(const parser &graph);
		place();
		~place();
		void naive_place();
		
	private:
		
		int place_LUT4(const vertex &,int);
		int place_DFF(const vertex &,int);
		
		int cell_count;
		
		map<int,vector<int> > all_driver;
};
