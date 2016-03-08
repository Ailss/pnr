#include "parser.h"

using namespace std;

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif

extern int OUT_PIN_[];
extern int IN_PIN_[];


parser::parser(char* file_name)
{
	memset(out_source,0,sizeof(out_source));
	v_count=0;
	pin_count=0;
	in_pin=0;
	out_pin=0;
	vtex.clear();
	vtex.push_back(vertex());
	pin_v.clear();
	pin_map.clear();
	
	//map PIN FALSE to 0, PIN TRUE to vertex -1
	pin_v[FALSE_PIN]=0;
	pin_v[TRUE_PIN]=-1;

		
	in_blif.open(file_name);
			
	if (in_blif.good())
	{
		cerr<<"Successfully Open the blif\n";
	}
	else
	{
		cerr<<"failed to open the blif file\n";
		exit(-1);
	}
			
			
}

parser::~parser()
{
	in_blif.close();
}

void parser::read_blif()
{
	char buffer[BUFSIZE];
	
	while (!in_blif.eof())
	{
		char buf[BUFSIZE];
		
		
		in_blif.getline(buffer,BUFSIZE);
		
		if (buffer[0]=='#' || buffer[0]=='\n' || buffer[0]==0)
			continue;
			
		if (buffer[0]!='.')
		{
			cerr<<"unknown content\n";
			cerr<<buffer<<endl;
			exit(-1);
		}
		
		//read .* to check the line attribute
		sscanf(buffer,"%s",buf);
		int bufl=strlen(buf);
		
		if (strcmp(buf,".names")==0)
		{
			deal_name(buffer+bufl);
		}
		else if (strcmp(buf,".gate")==0)
		{
			deal_gate(buffer+bufl);
		}
		else if (strcmp(buf,".end")==0)
		{
			return ;
		}
		else if (strcmp(buf,".model")==0)
		{
			sscanf(buffer+bufl,"%s",buf);
			model.assign(buf);
		}
		else if (strcmp(buf,".inputs")==0)
		{
			deal_inputs(buffer+bufl);
		}
		else if (strcmp(buf,".outputs")==0)
		{
			deal_outputs(buffer+bufl);
		}
	}
	
}

inline void next_nonspace(char* &c)
{
	while (*c!=' ' && *c!='\n' && *c!=0)
		c++;
	
	while (*c==' ')
		c++;
}
void parser::deal_gate(char* gate)
{
	char buf[BUFSIZE];
	vertex v;
	
	next_nonspace(gate);
	
	sscanf(gate,"%s",buf);
	v.name=buf;
	v_count++;
	
	if (strcmp(buf,"SB_LUT4")==0)
	{	
		v.type=SB_LUT4;
		
		
		//get input i0-i3
		for (int i=0;i<=3;i++)
		{
			next_nonspace(gate);
			sscanf(gate,"%*c%*c%*c%s",buf);
			help_lut4_input(buf,v);
		}
		
		//get output O
		next_nonspace(gate);
		sscanf(gate,"%*c%*c%s",buf);
		help_lut4_output(buf,v);
		
		//eliminate .attribute
		in_blif.getline(buf,BUFSIZE);
		if (strstr(buf,".attr")==0)
		{
			fprintf(stderr,"error: the line following .gate does not start with .attr\n");
			cerr<<buf<<endl;
			exit(-1);
		}
		
		
		//get the initiation of lut
		in_blif.getline(buf,BUFSIZE);
		if (strstr(buf,".param")==0)
		{
			fprintf(stderr,"error: the line following .attr does not start with .param\n");
			cerr<<buf<<endl;
			exit(-1);
		}

		
		char* pbuf=buf;
		next_nonspace(pbuf);
		next_nonspace(pbuf);
		int bufl=strlen(pbuf);
		
		int k=16;
		for (int j=0;j<16/bufl;j++)
		for (int i=0;i<bufl;i++)
		if (pbuf[i]=='1')
			v.sreg[--k]=1;
		else
			v.sreg[--k]=0;
			
		
		
	}
	else if (strcmp(buf,"SB_DFF")==0)
	{
		v.type=SB_DFF;
		
		//get clock
		next_nonspace(gate);
		sscanf(gate,"%*c%*c%s",buf);
		help_lut4_input(buf,v);
		
		//get the input D
		next_nonspace(gate);
		sscanf(gate,"%*c%*c%s",buf);
		help_lut4_input(buf,v);
		
		//get output Q
		next_nonspace(gate);
		sscanf(gate,"%*c%*c%s",buf);
		help_lut4_output(buf,v);
		
		//eliminate .attr
		in_blif.getline(buf,BUFSIZE);
		if (strstr(buf,".attr")==0)
		{
			fprintf(stderr,"warning: the line following .gate does not start with .attr\n");
			cerr<<buf<<endl;
			exit(-1);
		}
	}
	
	vtex.push_back(v);
}
void parser::help_lut4_output(char *buf,vertex &v)
{
	if (pin_map.find(string(buf))!=pin_map.end())	//if the pin appears in our pin map, we should use it
	{
		int pm=pin_map[string(buf)];
		//if the pin_map>0, this pin is driven by two units . an error occured here
		if (pm>0)
		{	
			if (pin_v.find(pm)!=pin_v.end())
				cerr<<"warning: PIN \""<<buf<<"\" is driven by two units.\n";
			else
			{
				pin_v[pm]=v_count;
				v.out_v.push_back(pm);
			}
		}
		else if (pm<MINIMUM_DEFAULT_VALUE)		//this ouput pin is a INPUT or OUTPUT pin
		{
			if (is_input(pm))
			{
				fprintf(stderr,"error: the output of vertex %d is the input of FPGA\n",v_count);
				exit(-1);
			}
			
			if (is_output(pm))
			{
				out_source[is_output(pm)]=v_count;
				v.out_v.push_back(pm);
			}
			else
			{
				fprintf(stderr,"error: the output of vertex %d is unknown default pin %d \n",v_count,pm);
				exit(-1);
			}
		}
		else	//out pin is a true or false
		{
			cerr<<"error: the output of vertex "<<v_count<<" is  connected to true or false\n";
			exit(-1);
		}
	}
	else 
	{
		pin_count++;
		pin_map[string(buf)]=pin_count;
		pin_v[pin_count]=v_count;
		v.out_v.push_back(pin_count);
	}
}

/*
*	generally, the pin_map is set as following
*	default pin <=0:
*	FALSE_PIN=0
*	TRUE_PIN=-1
*	INPUT_PIN 
*	OUTPUT_PIN
*	
*	pins defined by users >0
*	INTERCONN_PIN
*
*	if a pin has a negative count, this pin is not driven by any unit
*	
*/
void parser::help_lut4_input(char *buf,vertex &v)
{
	if (pin_map.find(string(buf))!=pin_map.end())	//if the pin appears in our pin map, we should use it
	{
		int pm=pin_map[string(buf)];
	
		v.in_v.push_back(pm);
			
	}
	else 		//this is the first time the pin appears, we need to create a pin num for it
	{
		pin_count++;
		pin_map[string(buf)]=pin_count;
		v.in_v.push_back(pin_count);
	}
}



void parser::deal_name(char* name)
{
	char buf1[BUFSIZE],buf2[BUFSIZE];
	
	if (sscanf(name,"%s%s",buf1,buf2)==1)
	{
		char buf[BUFSIZE];
		string tstr(buf1);
		
		in_blif.getline(buf,BUFSIZE);
		
		
		if (buf[0]=='.'||buf[0]==0)
		{
			//if the blif file name a FALSE PIN, the second line must not be 1
			//so we need to push the *get* pointer back 
			pin_map[tstr]=FALSE_PIN;
			in_blif.seekg(-strlen(buf)-1,ios::cur);	
		}
		else if (buf[0]=='1')
			pin_map[tstr]=TRUE_PIN;
	}
	else
	{
		char buf[BUFSIZE];
		in_blif.getline(buf,BUFSIZE);
		string str1(buf1),str2(buf2);
		
		
		if (pin_map.find(str1)==pin_map.end())
		{
			cerr<<"warning: in \"deal_name\" function, there exists a problem 1\n";
			
		}
		if (pin_map.find(str2)!=pin_map.end())
		{
			int pm=pin_map[str2];
			
			//if the output directly connects to the input
			//we need to use a lut to connect them
			if (is_output(pm))
			{
				vertex v;
				v_count++;
				v.name="SB_LUT4";
				v.type=SB_LUT4;
				pin_v[pm]=v_count;
				
				v.in_v.push_back(FALSE_PIN);
				v.in_v.push_back(FALSE_PIN);
				v.in_v.push_back(FALSE_PIN);
				v.in_v.push_back(pin_map[str1]);
				
				v.out_v.push_back(pm);
				v.sreg=0xff00;
				
				vtex.push_back(v);
			}
			
		}
		else
			pin_map[str2]=pin_map[str1];
	}
}

void parser::deal_inputs(char* inputs)
{
	char buf[BUFSIZE];

	
	next_nonspace(inputs);
	while (*inputs!=0)
	{
		in_pin++;
		if (in_pin>IN_PIN)
		{
			fprintf(stderr,"the number of inputs exceeds max input pins %d\n",IN_PIN);
			exit(-1);
		}
		
		sscanf(inputs,"%s",buf);
		pin_map[string(buf)]=IN_PIN_[in_pin];

		next_nonspace(inputs);
	}
	
	
}

void parser::deal_outputs(char* outputs)
{
	char buf[BUFSIZE];

	
	next_nonspace(outputs);
	while (*outputs!=0)
	{
		out_pin++;
		if (out_pin>OUT_PIN)
		{
			fprintf(stderr,"the number of ouputs exceeds max output pins %d\n",OUT_PIN);
			exit(-1);
		}
		
		sscanf(outputs,"%s",buf);
		pin_map[string(buf)]=OUT_PIN_[out_pin];

		next_nonspace(outputs);
	}
}









