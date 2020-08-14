# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/*
* Assume all geomitry optimization steps converged successfullly.
* This file extraxts structures of Each point in a Gaussian 
* relaxed-scan task" output file and write to a multi-frame xyz file.
*/

int main(int argc, char* argv[])
{
  typedef enum {False, True} booltype;
  const char* elements_list[] = {"",
   "H" , "He", "Li", "Be", "B" , "C" , "N" , "O" ,
   "F" , "Ne", "Na", "Mg", "Al", "Si", "P" , "S" ,
   "Cl", "Ar", "K" , "Ca", "Sc", "Ti", "V" , "Cr",
   "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge",
   "As", "Se", "Br", "Kr", "Rb", "Sr", "Y" , "Zr",
   "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd",
   "In", "Sn", "Sb", "Te", "I" , "Xe", "Cs", "Ba",
   "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd",
   "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf",
   "Ta", "W" , "Re", "Os", "Ir", "Pt", "Au", "Hg",
   "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra",
   "Ac", "Th", "Pa", "U" , "Np", "Pu", "Am", "Cm",
   "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf",
   "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn",
   "Nh", "Fl", "Mc", "Lv", "Ts", "Og"};
  char ifname[BUFSIZ] = "";
  const unsigned int uselesslines = 4;
  FILE* ifl = NULL;
  FILE* ofl = NULL;
  booltype sconverged = False;
  unsigned int nframes = 0;
  unsigned int natoms = 0;
  unsigned int ntotgeom = 0;
  unsigned int i = 0;
  unsigned int elem = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  long int readpos = 0;
  char l[BUFSIZ] = "";
  unsigned int t1 = 0, t2 = 0;
  char c = '\0';

  if (argc > 1)
    strcpy(ifname, argv[1]);
  else
  {
    puts("Input the path of the Gaussian relaxed-scan output file:");
    scanf("%[^\n]%c", ifname, & c);
  }

  ifl = fopen(ifname, "r");
  ofl = fopen("RelaxedScanSplit.xyz", "w");

  /*  obtain amount of atoms  */
  while (! feof(ifl))
  {
    fgets(l, BUFSIZ, ifl);
    if (! l)
      break;
    if (! strstr(l, "Input orientation"))
      continue;
    for(i = 0; i < uselesslines; i ++)
      fgets(l, BUFSIZ, ifl);
    while (True)
    {
      fgets(l, BUFSIZ, ifl);
      if (! strstr(l, "--------"))
      {
        natoms ++;
        continue;
      }
      break;
    }
    break;
  }
  fseek(ifl, 0, SEEK_SET);
  /*  read data  */
  while (! feof(ifl))
  {
    fgets(l, BUFSIZ, ifl);
    if (! l)
      break;
    if (strstr(l, "Input orientation"))
    {
      readpos = ftell(ifl);
      sconverged = False;
      while (! feof(ifl))
      {
        fgets(l, BUFSIZ, ifl);
        if ((strstr(l, "Input orientation")) || (! l))
        {
          if (l)
            ntotgeom ++;
          /*  check if last geometry should be used.  */
          fseek(ifl, readpos, SEEK_SET);
          if (sconverged)
          {
            /*  read data here  */
            nframes ++;
            fprintf(ofl, "%4d\n", natoms);
            fprintf(ofl, "structure %4d of relaxed scan\n", nframes);
            for (i = 0; i < uselesslines; i ++)
              fgets(l, BUFSIZ, ifl);
            for (i = 0; i < natoms; i ++)
            {
              fgets(l, BUFSIZ, ifl);
              sscanf(l, "%d %d %d %lf %lf %lf", & t1, & elem, & t2, & x, & y, & z);
              fprintf(ofl, " %-2s         %12.8lf  %12.8lf  %12.8lf\n", \
                      elements_list[elem], x, y, z);
            }
            fflush(ofl);
          }
          break;
        }
        if (strstr(l, "Optimization completed"))
          sconverged = True;
      }
    }   
  }

  fclose(ifl);
  fclose(ofl);
  ifl = NULL;
  ofl = NULL;

  printf("Total amount of atoms: %d\n", natoms);
  printf("Total frames of structures: %d\n", ntotgeom);
  printf("Total frames of converged structures: %d\n", nframes);
  printf("Done!\n");

  puts("Press ENTER to exit...");
  while((c = getchar()) != '\n' && c != EOF)
    ;

  return 0;
}


