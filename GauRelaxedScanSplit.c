# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>

/***********************************************************************
 * Assume all geomitry optimization steps converged successfullly.     *
 * This file extraxts structures of Each point in a Gaussian           *
 * relaxed-scan task" output file and write to a multi-frame xyz file. *
 ***********************************************************************/

int main(int argc, char const *argv[])
{
    char const *elements_list[] = {"", \
     "H" , "He", "Li", "Be", "B" , "C" , "N" , "O" , \
     "F" , "Ne", "Na", "Mg", "Al", "Si", "P" , "S" , \
     "Cl", "Ar", "K" , "Ca", "Sc", "Ti", "V" , "Cr", \
     "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", \
     "As", "Se", "Br", "Kr", "Rb", "Sr", "Y" , "Zr", \
     "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", \
     "In", "Sn", "Sb", "Te", "I" , "Xe", "Cs", "Ba", \
     "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", \
     "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", \
     "Ta", "W" , "Re", "Os", "Ir", "Pt", "Au", "Hg", \
     "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", \
     "Ac", "Th", "Pa", "U" , "Np", "Pu", "Am", "Cm", \
     "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", \
     "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn", \
     "Nh", "Fl", "Mc", "Lv", "Ts", "Og"};
    char ifl_name[BUFSIZ + 1] = "";
    int const num_useless_lines = 4;
    FILE *ifl = NULL;
    FILE *ofl = NULL;
    bool is_converged = false;
    int num_frames = 0;
    int num_atoms = 0;
    int num_total_geoms = 0;
    int i = 0;
    int element_index = 0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    size_t read_pos = 0;
    char buf[BUFSIZ + 1] = "";
    char c = '\0';
    char coordinates_locator[] = "Standard orientation";

    if (argc > 1)
    {
        strcpy(ifl_name, argv[1]);
    }
    else
    {
        printf("Input the path of the Gaussian relaxed scan output file: \n");
        fgets(ifl_name, BUFSIZ, stdin);
        if (ifl_name[strlen(ifl_name) - 1] == '\n')
        {
            ifl_name[strlen(ifl_name) - 1] = '\0';
        }
    }

    ifl = fopen(ifl_name, "rt");
    ofl = fopen("RelaxedScanSplit.xyz", "rt");
    if (ofl)
    {
        fprintf(stderr, "Warning! File \"RelaxedScanSplit.xyz\" already exists, moving to \"RelaxedScanSplit.bak\".\n");
        fclose(ofl);
        ofl = fopen("RelaxedScanSplit.bak", "rt");
        if (ofl)
        {
            fclose(ofl);
            remove("RelaxedScanSplit.bak");
        }
        rename("RelaxedScanSplit.xyz", "RelaxedScanSplit.bak");
    }
    ofl = fopen("RelaxedScanSplit.xyz", "wt");

    /* obtain amount of atoms and coordinates locator */
    while (fgets(buf, BUFSIZ, ifl))
    {
        if (strstr(buf, "NAtoms="))
        {
            break;
        }
    }
    sscanf(strstr(buf, "NAtoms=") + strlen("NAtoms="), "%d", & num_atoms);
    while (fgets(buf, BUFSIZ, ifl))
    {
        if (strstr(buf, "Optimization completed"))
        {
            break;
        }
        if (strstr(buf, "Input orientation"))
        {
            strcpy(coordinates_locator, "Input orientation");
            break;
        }
    }
    rewind(ifl);

    /* read data */
    while (fgets(buf, BUFSIZ, ifl))
    {
        if (strstr(buf, coordinates_locator))
        {
            read_pos = ftell(ifl);
            is_converged = false;
            while (fgets(buf, BUFSIZ, ifl))
            {
                if (strstr(buf, coordinates_locator))
                {
                    ++ num_total_geoms;
                    /* check if last geometry should be used. */
                    fseek(ifl, read_pos, SEEK_SET);
                    if (is_converged)
                    {
                        /* read data here */
                        ++ num_frames;
                        fprintf(ofl, "%4d\n", num_atoms);
                        fprintf(ofl, "structure %4d of relaxed scan\n", num_frames);
                        for (i = 0; i < num_useless_lines; ++ i)
                        {
                            fgets(buf, BUFSIZ, ifl);
                        }
                        for (i = 0; i < num_atoms; ++ i)
                        {
                            fgets(buf, BUFSIZ, ifl);
                            sscanf(buf, "%*d %d %*d %lf %lf %lf", & element_index, & x, & y, & z);
                            fprintf(ofl, " %-2s %15s %12.8lf    %12.8lf    %12.8lf\n", elements_list[element_index], "", x, y, z);
                        }
                        fflush(ofl);
                    }
                    break;
                }
                if (strstr(buf, "Optimization completed"))
                    is_converged = true;
            }
        }     
    }

    fclose(ifl);
    fclose(ofl);
    ifl = NULL;
    ofl = NULL;

    printf("Total amount of atoms: %d\n", num_atoms);
    printf("Total frames of structures: %d\n", num_total_geoms);
    printf("Total frames of converged structures: %d\n", num_frames);
    printf("Done!\n");

    printf("Press <ENTER> to exit ...\n");
    while((c = getchar()) != '\n' && c != EOF)
    {
        ;
    }

    return 0;
}


