#include "place.h"

using namespace std;

extern int OUT_PIN_[];
extern int IN_PIN_[];

place::place(const parser &graph)
{
	vtex=graph.vtex;
	memcpy(out_source,graph.out_source,sizeof(out_source));
	model=graph.model;
	pin_map=graph.pin_map;
	pin_v=graph.pin_v;
	v_count=graph.v_count;
	pin_count=graph.pin_count;
	in_pin=graph.in_pin;
	out_pin=graph.out_pin;
	

	
	groups[0]=64;
	groups[1]=64;
	groups[2]=64;
	groups[3]=48;
	
	cell_count=0;

	
	all_driver.clear();
	for (int i=1;i<=16;i++)
		all_driver[IN_PIN_[i]].push_back(240+i-1);
}

int get_most(int* groups,int n,int *ban)
{
	int mm,cnt;
	mm=-32767;
	for (int i=0;i<n;i++)
	if (groups[i]>mm && ban[i]==0)
	{
		mm=groups[i];
		cnt=i;
	}
	
	return cnt;
}

inline int get_group(int x)
{
	return x/64;
	
}

void place::naive_place()
{
	
	for (int i=1;i<=OUT_PIN;i++)
	if (out_source[i]>0)			//TO DO: check if this out pin is already placed. 
	{	
		
		if (vtex[out_source[i]].type==SB_LUT4)
			place_LUT4(vtex[out_source[i]],i-1);
		else if (vtex[out_source[i]].type==SB_DFF)
			place_DFF(vtex[out_source[i]],i-1);
		else
		{
			fprintf(stderr,"error: unknown type of logic unit %s\n",vtex[out_source[i]].name.c_str());
			exit(-1);
		}
		
	}
	
	for (int i=0;i<240;i++)
		sort(cells[i].in_v.begin(),cells[i].in_v.end());
}
void set_link(vertex &v,int input,int output)
{
	v.type=0;
	v.in_v.push_back(FALSE_PIN);
	v.in_v.push_back(FALSE_PIN);
	v.in_v.push_back(FALSE_PIN);
	v.in_v.push_back(input);
				
	v.out_v.push_back(output);
	v.sreg=0xff00;
}

int place::place_LUT4(const vertex &v,int pos=-1)
{
	int in[4];
	memset(in,0,sizeof(in));
	cell_count++;

	
	
	if (pos>=0)
	{
		groups[get_group(pos)]--;
		if (groups[get_group(pos)]<0)
		{
			cerr<<"error: run out of logic cells to place&route\n";
			exit(-1);
		}
		
		
		all_driver[v.out_v[0]].push_back(pos);
		
		
		cells[pos].sreg=v.sreg;
		
		for (int i=0;i<4;i++)			//here, i assume that the input of a lut will NOT be the outputs of the FPGA
		{
			if (v.in_v[i]!=0 && v.in_v[i]!=-1)
			{

				
				if (all_driver.find(v.in_v[i])!=all_driver.end())	//if this pin is already placed somewhere, we should try to use it
				{
					vector<int> &possible_places=all_driver[v.in_v[i]];
					vector<int>::iterator it;
					
					for (it=possible_places.begin();it!=possible_places.end();it++)		
					if (in[get_group(*it)]==0)						//ok, we can use this pin
					{
						in[get_group(*it)]=1;
						cells[pos].in_v.push_back(*it);
						break;
					}
					
					if (it==possible_places.end())
					{
						int j=get_most(groups,4,in);
						int link_p=j*64+groups[j]-1;
						in[j]=1;
						vertex tmp;
						set_link(tmp,v.in_v[i],v.in_v[i]);
						
						cells[pos].in_v.push_back(link_p);
						place_LUT4(tmp,link_p);
					}
					continue;
				}
				
				int j=get_most(groups,4,in);
				int link_p=j*64+groups[j]-1;
				
				in[j]=1;
				cells[pos].in_v.push_back(link_p);
				if (vtex[pin_v[v.in_v[i]]].type==SB_LUT4)
					place_LUT4(vtex[pin_v[v.in_v[i]]],link_p);
				else
					place_DFF(vtex[pin_v[v.in_v[i]]],link_p);
	
			}
			else
			{
				cells[pos].in_v.push_back(-1);
			}
		}
		
	}
	else
	{
		
	}
	return 0;
}

int place::place_DFF(const vertex &v,int pos=-1)
{
	
	if (pos>=0)
	{	
		all_driver[v.out_v[0]].push_back(pos);
		
		cells[pos].type=1;
		int v_id=pin_v[v.in_v[1]];
		
		if (vtex[v_id].type==SB_LUT4)
			place_LUT4(vtex[v_id],pos);
		else
		{
			vertex tmp;
			set_link(tmp,vtex[v_id].out_v[0],v.in_v[1]);
			place_LUT4(tmp,pos);
		}
	}
	return 0;
}

place::~place()
{
	
}




