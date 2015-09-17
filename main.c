#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmg_engine.h"
#include "function.h"



#define BUFFSIZE 1024
#define GP_NLEN 32
#define GP_NCHAR HMC_HINTLEN+1
#define DLIVES 15
#define DSELECT 0
#define NL 10
#define ADDLIVESTEP 5
#define NROW 10

#define SUCID 0
#define FALID 1
#define GSEXTID 2
#define GPEXTID 3
#define WINID 4
#define LOSID 5
#define NGID 6
#define SKID 7

#define HINT '.'
#define EXIT '-'
#define DELIM ':'
#define NEWGAME '+'
#define ADDLIVES '*'
#define SKIP '/'

static char buff[BUFFSIZE + 1];
static char gr[GP_NLEN][GP_NCHAR];

static const char STR_HMG_NAME[] = "LivesMan Game";
static const char STR_HMG_VERSION[] = "2.21";
static const char STR_HMG_YEAR[] = "2014";

static char *GFILE;
#define DFILE GFILE

static const char GPREFIX[] = "g";

static const char WIN[] = "[^ _ ^]  You win!";
static const char LOSE[] = "[T _ T]  You lose!";

static int playhmg (int ng);
static char *grappath (char *path);
static void showhelp (const char *str);
static void showscore (unsigned int score, int mlives, const char *gname,
		       const char *datafile);


static assem *gas, *das;
static hms_game *game;
static unsigned int lives = DLIVES, maxlives = DLIVES;
static unsigned int gcount = 1, newgame = 1;

static unsigned int gselected, dselected;
static unsigned int nstep, addlivestep = ADDLIVESTEP;
static memman mm;

int
main (int argc, char *argv[])
{
  hms_group *gptr;
  hms_data *dptr;
  FILE *gfp, *dfp;

  int i, j;

/*******************************************************************************************************************/

  if (argc == 2 || argc == 3 || argc == 4)
    {

      DFILE = argv[1];
      GFILE = argv[1];

      if (argc == 3 || argc == 4)
	{

	  if (!isUint (argv[2]))
	    {
	      fprintf (stderr,
		       "ERR 1 ARG: %s is not positive number\n\n", argv[2]);
	      showhelp (grappath (argv[0]));
	      return 1;
	    }
	  else if ((maxlives = s2ui (argv[2], 10)) < 1)
	    {
	      fprintf (stderr, "ERR 1 ARG: %s is must >= 1\n\n", argv[2]);
	      showhelp (grappath (argv[0]));
	      return 1;
	    }
	}

      if (argc == 4)
	{

	  if (!isUint (argv[3]))
	    {
	      fprintf (stderr,
		       "ERR 2 ARG: %s is not positive number\n\n", argv[3]);
	      showhelp (grappath (argv[0]));
	      return 1;
	    }
	  else if ((addlivestep = s2ui (argv[3], 10)) < 1)
	    {
	      fprintf (stderr, "ERR 2 ARG: %s is must >= 1\n\n", argv[3]);
	      showhelp (grappath (argv[0]));
	      return 1;
	    }

	}

    }

  else
    {
      fprintf (stderr, "ERR ARG: Too many arguements\n\n");
      showhelp (grappath (argv[0]));
      return 1;
    }

/*******************************************************************************************************************/
  mm_init (&mm);
/***********************************************/
  if (!(gas = hmf_init (0, &mm)))
    {
      fprintf (stderr,
	       "ERR INIT GAS: Can not alloc memory for group assembly\n");
      mm_freeall (&mm);
      return 1;
    }

  if (!(gfp = fopen (GFILE, "r")))
    {
      perror (GFILE);
      mm_freeall (&mm);
      return 1;
    }

  while (fgets (buff, BUFFSIZE, gfp))
    {
      cutnl (buff, 0);
      i = grapstring (cutcharbe (buff, 0x20), DELIM, GP_NLEN, GP_NCHAR, gr);

      if (i < 0)
	{
	  hmf_free (&mm, gas);
	  fclose (gfp);
	  fprintf (stderr, "ERR FILE: %s\n", GFILE);

	  if (i == -1)
	    {
	      fprintf (stderr,
		       "ERR FILE: %s :item is limited to <%u> key numbers\n",
		       GFILE, GP_NLEN - 1);
	      return 1;
	    }

	  else if (i == -2)
	    {
	      fprintf (stderr,
		       "ERR FILE: %s :item is limited to <%u> charecter numbers\n",
		       GFILE, GP_NCHAR - 1);
	      return 1;
	    }

	}

      if (i > 0)
	{

	  if (!(j = strcmp (gr[0], GPREFIX)) && (i == 2))
	    {

	      for (j = 1; j < i; j++)
		{

		  if (!(gptr = NEWGROUP ()))
		    {
		      fprintf (stderr,
			       "ERR NEWGRP: Can not alloc memory for group\n");
		      mm_freeall (&mm);
		      fclose (gfp);
		      return 1;
		    }

		  strcpy (gptr->name, gr[j]);

		  if (!hmf_add (gas, gptr, &mm))
		    {
		      fprintf (stderr,
			       "ERR ADDGRP: Can not add linklist for group\n");
		      mm_freeall (&mm);
		      fclose (gfp);
		      return 1;
		    }

		  GPTR (gas->end->ptr)->gid = gas->end->id;

		}
	    }

	  else if ((j && (i == 2)) || (!j && (i != 2)))
	    {
	      fprintf (stderr,
		       "ERR ADDGROUP: File < %s >\n\t\tMSG: < %s > must be in first parameter\n\t\tMSG: numers of data must be 3  and numers of group must be 2\n\n",
		       DFILE, GPREFIX);

	      fprintf (stderr, "ERR DETAIL: ");
	      for (j = 0; j < i; j++)
		{
		  fprintf (stderr, "%s ", gr[j]);
		}
	      fprintf (stderr, "\n");

	      mm_freeall (&mm);
	      fclose (gfp);
	      return 1;
	    }

	  else if ((i > 3) || (i == 1))
	    {

	      fprintf (stderr,
		       "ERR ADDOBJECT: File < %s >\n\t\tMSG: Numers of data must be 3  and numers of group must be 2\n\n",
		       DFILE);
	      fprintf (stderr, "ERR DETAIL: ");
	      for (j = 0; j < i; j++)
		{
		  fprintf (stderr, "%s ", gr[j]);
		}
	      fprintf (stderr, "\n");

	      mm_freeall (&mm);
	      fclose (gfp);
	      return 1;
	    }

	}

    }

  fclose (gfp);

  if (!hmf_getngroup (gas))
    {
      fprintf (stderr,
	       "ERR INITGAME: FILE <%s> :Nuber of group must'n be <0>\n",
	       grappath (GFILE));
      mm_freeall (&mm);
      return 1;
    }


/*******************************************************************************************************************/

  if (!(das = hmf_init (1, &mm)))
    {
      mm_freeall (&mm);
      fprintf (stderr,
	       "ERR INIT DAS: Can not alloc memory for data assembly\n");
      return 1;
    }

  if (!(dfp = fopen (DFILE, "r")))
    {
      perror (DFILE);
      mm_freeall (&mm);
      return 1;
    }

  while (fgets (buff, BUFFSIZE, dfp))
    {
      cutnl (buff, 0);
      i = grapstring (cutcharbe (buff, 0x20), DELIM, GP_NLEN, GP_NCHAR, gr);


      if (i > 0)
	{

	  if (i == 3)
	    {

	      if (!(dptr = NEWDATA ()))
		{
		  fprintf (stderr,
			   "ERR NEWDATA: Can not alloc memory for data\n");
		  mm_freeall (&mm);
		  fclose (dfp);
		  return 1;
		}
	      if (!isUint (gr[0]))
		{
		  fprintf (stderr,
			   "ERR ADDDATA: < %s > :First parameter of data is not a number\n",
			   DFILE);
		  fprintf (stderr, "ERR DETAIL: ");
		  for (j = 0; j < i; j++)
		    {
		      fprintf (stderr, "%s ", gr[j]);
		    }
		  fprintf (stderr, "\n");

		  mm_freeall (&mm);
		  fclose (dfp);
		  return 1;
		}

	      if (!strlen (gr[1]))
		{
		  fprintf (stderr,
			   "ERR ADDDATA: < %s > :String length of guess word must not be 0\n",
			   grappath (DFILE));
		  free (dptr);
		  mm_freeall (&mm);
		  fclose (dfp);
		  return 1;
		}

	      dptr->gid = s2ui (gr[0], 10);
	      strcpy (dptr->name, gr[1]);
	      strcpy (dptr->hint, gr[2]);

	      if (!hmf_add (das, dptr, &mm))
		{
		  fprintf (stderr,
			   "ERR ADDDATA: Can not add linklist for data\n");
		  mm_freeall (&mm);
		  fclose (dfp);
		  return 1;
		}

	    }

	}

     /**/}
  fclose (dfp);

/**********************************************************************************************************************/

/**********************************************************************************************************************/

  hmf_srandom ();

  while ((j = playhmg (newgame)) > GPEXTID)
    {
      if (j == WINID)
	{			/*   WIN  */
	  gcount++;
	  lives = game->lives;
	  newgame = 0;
	  mm_free (&mm, game);
	}

      else if (j == NGID)
	{
	  showscore (gcount - 1, maxlives,
		     hmf_getgroup (gas, gselected)->name, grappath (GFILE));
	  gcount = 1;
	  newgame = 1;
	  mm_free (&mm, game);
	}

      else if (j == SKID)
	{
	  lives = game->lives;
	  newgame = 0;
	  mm_free (&mm, game);
	}

      else if (j == LOSID)
	{			/*  LOSE  */
	  showscore (gcount - 1, maxlives + nstep * addlivestep,
		     hmf_getgroup (gas, gselected)->name, grappath (GFILE));

	  do
	    {

	      puts ("\nDo you wnat to continue this game?");
	      printf ("Answer->(Y/n) ");
	      fgets (buff, BUFFSIZE, stdin);
	      cutnl (buff, 0);
	      if (!strlen (buff))
		{
		  buff[0] = 'y';
		}
	    }
	  while ((strlen (buff) > 1) || (buff[0] != 'y' && buff[0] != 'n'));

	  if (buff[0] == 'n')
	    {			/* Press N */
	      break;
	    }
	  else if (buff[0] == 'y')
	    {			/* Press Y */
	      gcount = 1;
	      newgame = 1;
	      mm_free (&mm, game);
	    }

	}
    }

  if (j == GPEXTID)
    {
      showscore (gcount - 1, maxlives + nstep * addlivestep,
		 hmf_getgroup (gas, gselected)->name, grappath (GFILE));
    }

  if ((j != FALID) && (j != GSEXTID))
    {
      mm_free (&mm, game);
    }

  mm_freeall (&mm);

  return j == FALID ? 1 : 0;
}

/***********************************************************************************************************************/

static int
playhmg (int ng)
{
  linklist *ptr;
  int i, hint = 0;
  unsigned int n;

  if (ng == 1)
    {
      do
	{

	  puts
	    ("\n********************* List of groups *********************");
	  printf ("                      [ %s ]\n\n", grappath (GFILE));
	  for (n = 1, ptr = gas->begin; ptr; ptr = ptr->forw, n++)
	    {

	      printf ("%u = %s (%u)\n", GPTR (ptr->ptr)->gid,
		      GPTR (ptr->ptr)->name, hmf_getndata (das,
							   GPTR
							   (ptr->ptr)->gid));

	      if (!(n % (NROW)))
		{
		  printf ("Press Enter to continue");

		  fgets (buff, BUFFSIZE, stdin);

		}

	    }

/*
if((n-1)%NCOL)   {putchar(NL);}
*/

	  putchar (NL);
	  printf ("Default = %s\n", hmf_getgroup (gas, DSELECT)->name);
	  printf ("Which one ? or type (%c) to exit.\n", EXIT);
	  printf ("Answer -> ");

	  fgets (buff, BUFFSIZE, stdin);
	  putchar (NL);
	  cutnl (buff, 0);

	  if ((strlen (buff) == 1) && (buff[0] == EXIT))
	    {
	      return GSEXTID;
	    }
	  else if (!strlen (buff))
	    {
	      buff[0] = '0';
	    }

	}
      while ((!isUint (buff))
	     || ((n = s2ui (buff, 10)) > (hmf_getngroup (gas) - 1))
	     || (!hmf_getndata (das, n)));
      gselected = n;
      lives = maxlives;
      nstep = 0;
    }
  /* End of new game procedure */
  dselected = hmf_random (0, hmf_getndata (das, gselected) - 1);
  game = hmf_initgame (hmf_getdata (das, gselected, dselected), lives, &mm);
  if (!game)
    {
      fprintf (stderr, "ERR INITGAME: Can not init for a game\n");

      return FALID;
    }

  while (1)
    {

      do
	{
	  printf
	    ("\n********************* %u Game section *********************\n",
	     gcount);

	  puts (hmf_getgroup (gas, gselected)->name);
	  printf
	    ("(%c)Hint (%c)New Game (%c)Add %u lives (%c)Skip (%c)Exit\n",
	     HINT, NEWGAME, ADDLIVES, addlivestep, SKIP, EXIT);

	  if (hint)
	    {
	      printf ("\t\t\t\tHint: %s", game->data->hint);
	      hint = 0;
	    }

	  printf ("\nLives %u/%u\n\n", game->lives,
		  maxlives + nstep * addlivestep);
	  printf ("%s\n\n", game->name);
	  printf ("Type your char -> ");

	  fgets (buff, BUFFSIZE, stdin);
	  cutnl (buff, 0);
	}
      while (strlen (buff) != 1);

      if (buff[0] == HINT)
	{

	  if (game->lives > 1)
	    {
	      hint = 1;
	      game->lives--;
	    }
	  i = 0;
	}

      else if (buff[0] == EXIT)
	{
	  return GPEXTID;
	}

      else if (buff[0] == NEWGAME)
	{
	  return NGID;
	}

      else if (buff[0] == ADDLIVES)
	{
	  nstep++;
	  game->lives += addlivestep;

	  game->win = 0;
	  i = 0;
	}

      else if (buff[0] == SKIP)
	{

	  if (game->lives > 1)
	    {
	      game->lives--;
	      return SKID;
	    }
	  i = 0;

	}

      else
	{
	  i = hmf_gameplay (game, buff[0]);
	}

      if (i == 1)
	{
	  putchar (NL);
	  printf ("%s\n\t\t\t< %s >\n", game->name, WIN);
	  return WINID;
	}
      else if (i == -1)
	{
	  putchar (NL);
	  printf ("%s\n\t\t\t< %s >\n", hmf_solve (game)->name, LOSE);
	  return LOSID;
	}

    }
  return SUCID;
}


static char *
grappath (char *path)
{				/* return -1 if NULL ptr, 0 if sep was not found  and position of filename if success */
  int i, j = -1;
  if (!path)
    {
      return ((char *) 0);
    }
  for (i = 0; path[i]; i++)
    {
      if ((path[i] == '\\') || (path[i] == '/'))
	{
	  j = i;
	}
    }
  return path + ((j == -1) ? 0 : j + 1);

}

static void
showhelp (const char *str)
{
  fprintf (stderr, "%s version %s published in %s\n\n", STR_HMG_NAME,
	   STR_HMG_VERSION, STR_HMG_YEAR);

  fprintf (stderr, "USAGE: %s gamefile\n", str);

  fprintf (stderr, "\tor\n");

  fprintf (stderr, "USAGE: %s gamefile nlives\n", str);

  fprintf (stderr, "\tor\n");

  fprintf (stderr, "USAGE: %s gamefile nlives nstep\n\n", str);
}

static void
showscore (unsigned int score, int mlives, const char *gname,
	   const char *datafile)
{
  printf ("\n<< Game data: %s >>\n", datafile);
  printf ("<< You have selected %s group >>\n", gname);
  printf ("<< You have %u wins in %u lives >>\n", score, mlives);
}
