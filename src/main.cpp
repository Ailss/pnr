
#ifndef PARSER_H
#define PARSER_H
#include "parser.h"
#endif

#include "place.h"


using namespace std;

extern void pin_init();


place *place_tool;



int main(int argc,char* argv[])
{
	if (argc==1)
	{
		cerr<<"pnr.elf [.blif]\n";
		return 0;
	}
	parser blif_parser(argv[1]);

	
	pin_init();
	
	blif_parser.read_blif();
	
	
	for (int i=1;i<=blif_parser.v_count;i++)
	{
		//fprintf(stderr,"vertex %d type %s inputs %d %d %d %d\n",i,blif_parser.vtex[i].name.c_str(),blif_parser.vtex[i].in_v[0],blif_parser.vtex[i].in_v[1],blif_parser.vtex[i].in_v[2],blif_parser.vtex[i].in_v[3]);
		
		fprintf(stderr,"vertex %d type %s ",i,blif_parser.vtex[i].name.c_str());
		
		fprintf(stderr,"inputs ");
		for (vector<int>::iterator it=blif_parser.vtex[i].in_v.begin();it!=blif_parser.vtex[i].in_v.end();it++)
			fprintf(stderr,"%d ",*it>0?blif_parser.pin_v[*it]:*it);
		
		fprintf(stderr,"outputs ");
		for (vector<int>::iterator it=blif_parser.vtex[i].out_v.begin();it!=blif_parser.vtex[i].out_v.end();it++)
			fprintf(stderr,"%d ",*it>0?blif_parser.pin_v[*it]:*it);
		if (blif_parser.vtex[i].type==SB_LUT4)
			cerr<<blif_parser.vtex[i].sreg;
		fputc('\n',stderr);
	}
	
	fprintf(stderr,"outputs:");
	for (int i=1;i<=16;i++)
	if (blif_parser.out_source[i]>0)
		fprintf(stderr," %d",blif_parser.out_source[i]);
	fprintf(stderr,"\n");
	
	place_tool=new place(blif_parser);
	place_tool->naive_place();
	for (int i=0;i<240;i++)
	if (place_tool->cells[i].in_v.size())
	{
		fprintf(stderr,"logic cell %d: ",i);
		for (vector<int>::iterator it=place_tool->cells[i].in_v.begin();it!=place_tool->cells[i].in_v.end();it++)
			fprintf(stderr,"%d ",*it);
			
		cerr<<place_tool->cells[i].sreg;
		fputc(' ',stderr);
		cerr<<place_tool->cells[i].type;
		fputc('\n',stderr);
	}
	for (int i=0;i<240;i++)
	if (place_tool->cells[i].in_v.size())
	{
		for (int j=3;j>=0;j--)
		if (place_tool->cells[i].in_v[j]/64==j && place_tool->cells[i].in_v[j]>0)
			fprintf(stdout,"%c",(char)(place_tool->cells[i].in_v[j]%64));
		else
			fprintf(stdout,"%c",(char)0);
		
	}
	else
	{
		fprintf(stdout,"%d",0);
	}

	for (int i=0;i<240;i++)
	if (place_tool->cells[i].in_v.size())
	{
		int x=0;
		x=(int)place_tool->cells[i].sreg.to_ulong() ;
		x=x<<1;
		x=x|place_tool->cells[i].type;
		x=x<<1;
		fprintf(stdout,"%d",x);
	}
	else
	{
		fprintf(stdout,"%d",0);
	}
	delete place_tool;

	return 0;
}
