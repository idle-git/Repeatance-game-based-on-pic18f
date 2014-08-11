//#include "rand2.h" 

static unsigned long int _seed = 1;
unsigned int rv;
int rand2(void)
{
	_seed = _seed * 1103515245UL + 12345UL;
    rv = (unsigned int)(_seed/65536UL) % 32768UL;
    if ( (rv%2) == 0 )
	{
        return 2;
	}
    else
	{
        return 1;
	}
}

void srand2(unsigned int seed)
{
	_seed = seed;
}
