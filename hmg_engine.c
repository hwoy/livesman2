#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "liteList/linklist.h"
#include "mem/mem.h"
#include "hmg_engine.h"


#ifdef _DEVRAND_
static const char	DEVRAND[] = "/dev/random";
#endif

/*********************************** intit random functions ********************************/
void
hmf_srandom(void)
{
	srand(time(NULL));
}

unsigned int
hmf_rand()
{
#ifdef _DEVRAND_
        FILE           *fp;
        unsigned int             i,j,k;
        fp = fopen(DEVRAND, "rb");
        if (!fp)
                return 0;
		for(k=0,j=0;j<sizeof(i);j+=sizeof(char),k++)
        ((char *)&i)[k] = fgetc(fp);
	
        fclose(fp);
        return i;
#else
			return	rand   ();
#endif
}

/*********************************** random functions ********************************/
unsigned int
hmf_random(unsigned int min, unsigned int max)
{
	return min <= max ? min + (hmf_rand() % (max - min + 1)) : -1;
}



assem          *
hmf_init(int id, memman * mm)
{
	assem          *asmb;

	if (!(asmb = (assem *) mm_malloc(mm, sizeof(assem)))) {
		return (assem *) 0;
	}
	l2_init(asmb, id);
	return asmb;
}

/*********************************** free functions ********************************/
void
hmf_free(assem * asmb, memman * mm)
{
	linklist       *tmp, *ptr;
	for (ptr = asmb->begin; ptr; ptr = ptr->forw) {
		tmp = ptr->forw;
		mm_free(mm, tmp->ptr);
		mm_free(mm, tmp);
	}

	mm_free(mm, asmb);

}


int
hmf_add(assem * asmb, void *obj, memman * mm)
{
	linklist       *list;
	if (!(list = mm_malloc(mm, sizeof(linklist))))
		return 0;
	list->ptr = obj;
	l2_add(asmb, list);
	return 1;
}

/*********************************** get number of data functions ********************************/
unsigned int
hmf_getndata(assem * asmb, unsigned int gid)
{
	linklist       *ptr;
	unsigned int	n;

	for (n = 0, ptr = asmb->begin; ptr; ptr = ptr->forw) {

		if (gid == DPTR(ptr->ptr)->gid) {
			n++;
		}
	}
	return n;
}

/*********************************** get data functions ********************************/
hms_data       *
hmf_getdata(assem * asmb, unsigned int gid, unsigned int index)
{
	linklist       *ptr;
	unsigned int	n;

	for (n = 0, ptr = asmb->begin; ptr; ptr = ptr->forw) {


		if (gid == DPTR(ptr->ptr)->gid) {
			if (index == n) {
				return DPTR(ptr->ptr);
			}
			n++;
		}
	}

	return (hms_data *) 0;
}

/*********************************** init game functions ********************************/
hms_game       *
hmf_initgame(hms_data * data, unsigned int lives, char unch, memman * mm)
{
	hms_game       *game;
	unsigned int	i;

	if (!(game = (hms_game *) mm_malloc(mm, sizeof(hms_game)))) {
		return (hms_game *) 0;
	}
	game->data = data;
	game->lives = lives;

	game->win = 0;
	game->unch = unch;

	for (i = 0; game->data->name[i]; i++) {
		game->name[i * 2] = unch;
		game->name[i * 2 + 1] = 0x20;
	}
	game->name[i * 2] = 0;

	return game;
}

/*********************************** get group functions ********************************/
hms_group      *
hmf_getgroup(assem * asmb, unsigned int gid)
{
	linklist       *ptr;

	for (ptr = asmb->begin; ptr; ptr = ptr->forw) {


		if (gid == GPTR(ptr->ptr)->gid) {
			return GPTR(ptr->ptr);
		}
	}
	return (hms_group *) 0;
}

/*********************************** get number of group functions ********************************/
unsigned int
hmf_getngroup(assem * asmb)
{
	linklist       *ptr;
	unsigned int	n;

	for (n = 0, ptr = asmb->begin; ptr; ptr = ptr->forw, n++) {
	}

	return n;
}

/*********************************** gameplay functions ********************************/
int
hmf_gameplay(hms_game * game, char ch)
{

	int		i         , j;

	if (game->lives < 1) {
		game->win = -1;
		return -1;
	}
	for (i = 0, j = 0; game->data->name[i]; i++) {

		if (ch == game->data->name[i]) {
			game->name[i * 2] = ch;
			j++;
		}
	}

	if (!j) {
		game->lives--;
	}
	for (i = 0, j = 1; game->data->name[i]; i++) {

		if (game->name[i * 2] != game->data->name[i]) {
			j = 0;
			break;
		}
	}

	if (j) {
		game->win = 1;
	}
	if (!game->lives) {
		game->win = -1;
	}
	return game->win;
}


hms_game       *
hmf_solve(hms_game * game)
{
	int		i;
	for (i = 0; game->data->name[i]; i++) {
		game->name[i * 2] = game->data->name[i];
	}
	game->name[i * 2] = game->data->name[i];
	return game;
}
