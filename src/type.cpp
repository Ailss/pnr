#include "type.h"

using namespace std;


int IN_PIN_[IN_PIN+1];
int OUT_PIN_[OUT_PIN+1];

void pin_init()
{
	int pin=-1;
	for (int i=1;i<=IN_PIN;i++)
		IN_PIN_[i]=--pin;
	for (int i=1;i<=OUT_PIN;i++)
		OUT_PIN_[i]=--pin;
}

int is_input(int x)
{
	if (x>=IN_PIN_[IN_PIN] && x<=IN_PIN_[1])
		return -x+IN_PIN_START;
	return 0;
}

int is_output(int x)
{
	if (x>=OUT_PIN_[OUT_PIN] && x<=OUT_PIN_[1])
		return -x+OUT_PIN_START;
	return 0;
}
