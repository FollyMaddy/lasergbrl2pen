/* Program : lasergbrl2pen.c
* Version : 1
* Use : 
* This program is created to change lasergbrl g-code into g-code for marlin based cnc.
* The laserintesity is converted into Z-depth.
* An Up-Down-Up motion for each coordinate is added (G80/G81 simulation)
* Compilation on linux :
* gcc -o  lasergbrl2pen lasergbrl2pen.c
* Also compiling for windows 32 and 64 bit on linux is possible with mingw-w64 (sudo apt-get install mingw-w64):
* i686-w64-mingw32-gcc -o lasergbrl2pen_32.exe lasergbrl2pen.c
* x86_64-w64-mingw32-gcc -o lasergbrl2pen_64.exe lasergbrl2pen.c
* Nothing has to be installed to run this on windows.
* Can be run is cmd in the commandline or open your file with lasergbrl2pen_64.exe.
*
* Author : Folkert van der Meulen
* Date   : 13/09/2019
*
* Copyright 2019 Folkert van der Meulen
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/********************************************************
 * This program reads a file specified by the user      *
 * as a command line argument and changes the           *
 * lasergbrl gcode file to a working pen plotter        *
 * gcode file for arduino marlin based firmwares        *
 * (in the future it also has to work with cutter gcode)*
 * Works with all gcodefiles from lasergbrl             *
 ********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1

//this function will get the string array
//and split into the words, seprated by spaces

int getLaserStrength(char *base, char target[10][20])
{
	int n=0,i,j=0;
	
	for(i=0;TRUE;i++)
	{
		if(base[i]!='Z'){
			target[n][j++]=base[i];
		}
		else{
			target[n][j++]='\0';//insert NULL
			n++;
			j=0;
		}
		if(base[i]=='\0')
		    break;
	}
	return n;
	
}


int main(int argc, char *argv[])
{
 FILE *in_file;
 FILE *out_file;
 char buf[1000];
 char *filename_in;
 char *filename_out;
 int Z_depth;
 int Z_retraction;
 int Z_depth_digit = 0;
 int Z_retraction_digit = 0;
 int counter;
 int LaserStrength;
 int Z_depthRate;
 int Z_depthRate_digit;
 char ChoiceInverseDepth = 'n'; //choise is turned to off for now
 char ChoiceUpDownUp;

   // Check if a filename has been specified in the command
   if (argc < 2)
   {
      printf("Missing Filename\n");
      return(1);
   }
   else
   {
      filename_in = argv[1];
      printf("Filename : %s\n", filename_in);
   }

   // Open file in read-only mode
   in_file = fopen(filename_in,"r"); 

   // Enter Z-axis-retraction-height for plotter
   printf("Enter Z-axis-retraction-height in mm (pen-plotter is -4.0 and 3d-printer-pen-plotter is 1.0):");
   scanf("%d.%d", &Z_retraction, &Z_retraction_digit);  
   
   // Enter Z-axis-depth for plotter
   printf("Enter Z-axis-depth in mm (pen-plotter is -5.0 and 3d-printer-pen-plotter is 0.0):");
   scanf("%d.%d", &Z_depth, &Z_depth_digit);  

   // Enter y to add Z-axis Up->Down->Up motion for lasergbrl centreline-outputfile (G81/G80 Simulation). (to make drills, neadle pins or pen dots)
   printf("Enter y/n to add Z-axis Up->Down->Up motion for lasergbrl centreline-outputfile pcb-maker-test (G81/G80 Simulation):");
   scanf("%s", &ChoiceUpDownUp);


   //choise is turned to off for now
   // Enter y to inverse Z-axis motion. (Laser intensity to Z-axis conversion is inverted)
   //printf("Enter y/n to inverse Z-axis motion (Laser intensity to Z-axis conversion is inverted):");
   //scanf("%s", &ChoiceInverseDepth);

   // If file opened successfully, then print the contents
   if ( in_file )
   {
      filename_out = (filename_in);
      //concatenates str1 and str2 and resultant string is stored in str1.
      strcat(filename_out,".out.nc");
      printf(filename_out);
      printf("\n");
      out_file = fopen(filename_out,"w");
      printf("Output file contents:\n");
      while (fgets(buf,1000, in_file)!=NULL)
		 if (strstr(buf, "G0 X0 Y0") == buf)
         {
         // starts with G0 X0 Y0
         // add Z0 so that pen will retract to 0 after gcode has executed
         printf("G0 X0 Y0 Z0\r\n");
         fprintf(out_file, "G0 X0 Y0 Z0\r\n");
         }
         else if (strstr(buf, "G0X0Y0") == buf)
         {
         // starts with G0 X0 Y0
         // add Z0 so that pen will retract to 0 after gcode has executed
         printf("G0X0Y0Z0\r\n");
         fprintf(out_file, "G0X0Y0Z0\r\n");
         }
         //fast fix for line to line conversion // can change for 1 bit conversions
         else if (strstr(buf, "S0") == buf)
         {
         // starts with S0
         // add G0 so that pen will retract to  Z_retraction-height
         printf("G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         fprintf(out_file, "G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         }
         else if (strstr(buf, "M3 S0") == buf)
         {
         // starts with M3 S0
         //printf(";next line changed into => G00 Z5.0000 ;original => %s",buf);
         //fprintf(out_file, ";next line changed into => G00 Z5.0000 ;original => %s",buf);
         printf("G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         fprintf(out_file, "G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         }
         else if (strstr(buf, "M3") == buf)
         {
         // starts with M3
         //printf(";next 2 lines changed to => G0 Z0.5000 and G1 Z5.0000 F600 S10000;original => %s",buf);
         //fprintf(out_file, ";next 2 lines changed into => G0 Z0.5000 and G1 Z5.0000 F600 S10000;original => %s",buf);
         
         //next 2 lines seem to be not nessasary // earlier kept because of some compatibility perhaps Z-axis not retracting always
         //printf("G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         //fprintf(out_file, "G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         
          	if (ChoiceUpDownUp == 'y' || ChoiceUpDownUp == 'Y')
			{
			printf("\n");
			fprintf(out_file, "\n");
			}
			else if (ChoiceUpDownUp == 'n' || ChoiceUpDownUp == 'N')
			{
			printf("G1 Z%d.%d000 F600\r\n",Z_depth, Z_depth_digit);
			fprintf(out_file, "G1 Z%d.%d000 F600\r\n",Z_depth, Z_depth_digit);
			}                        
         }
         else if (strstr(buf, "M5") == buf)
         {
         // starts with M5
         //printf(";next line changed into => G00 Z5.0000 ;original => %s",buf);
         //fprintf(out_file, ";next line changed into => G00 Z5.0000 ;original => %s",buf);
         printf("G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         fprintf(out_file, "G0 Z%d.%d000\r\n",Z_retraction, Z_retraction_digit);
         }
         else if (strstr(buf, "X") == buf)
         {
         // starts with X and if line contains an S, the S will change into Z
         counter = 0;
         while(buf[counter] != '\0')
         {
			if(buf[counter] == 'S')
			{
			buf[counter] = 'Z';
			}
			else if(buf[counter] == '\n')
			{
			buf[counter] = ' ';			
			}			
			counter++;
			}
         	if (ChoiceUpDownUp == 'y' || ChoiceUpDownUp == 'Y')
			{
			//printf(";next line changes X into G1 X and S into ZrZdZr\n");
			//fprintf(out_file, ";next line changed X into G1 X and S into Z\n");
			printf("G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			printf("G1 %sZ%d.%d\n",buf, Z_depth, Z_depth_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_depth, Z_depth_digit);
			printf("G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			}
			else if (ChoiceUpDownUp == 'n' || ChoiceUpDownUp == 'N')
			{
			//printf(";next line changed X into G1 X and S into Z\n");
			//fprintf(out_file, ";next line changed X into G1 X and S into ZrZdZr\n");
			printf("G1 %s\n",buf);
			fprintf(out_file, "G1 %s\n",buf);
			}
         }
         else if (strstr(buf, "Y") == buf)
         {
         // starts with Y and if line contains an S, the S will change into Z
         counter = 0;
         while(buf[counter] != '\0')
         {
			if(buf[counter] == 'S')
			{
			buf[counter] = 'Z';
			}
			else if(buf[counter] == '\n')
			{
			buf[counter] = ' ';			
			}			
			counter++;
			}
         	if (ChoiceUpDownUp == 'y' || ChoiceUpDownUp == 'Y')
			{
			//printf(";next line changes Y into G1 Y and S into ZrZdZr\n");
			//fprintf(out_file, ";next line changed Y into G1 Y and S into ZrZdZr\n");
			printf("G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			printf("G1 %sZ%d.%d\n",buf, Z_depth, Z_depth_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_depth, Z_depth_digit);
			printf("G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			fprintf(out_file, "G1 %sZ%d.%d\n",buf, Z_retraction, Z_retraction_digit);
			}
			else if (ChoiceUpDownUp == 'n' || ChoiceUpDownUp == 'N')
			{
			//printf(";next line changed X into G1 X and S into Z\n");
			//fprintf(out_file, ";next line changed X into G1 X and S into Z\n");
			printf("G1 %s\n",buf);
			fprintf(out_file, "G1 %s\n",buf);
			}
         }
         else if (strstr(buf, "S") != NULL)
         {	
         // contains S value and is not changed by earlier else if's // has to change in the future
         counter = 0;
         while(buf[counter] != '\0')
         {
			if(buf[counter] == 'S')
			{
			buf[counter] = 'Z';
			}
			counter++;
			}
		 int n; //number of words
		 int i; //loop counter 
		
		 char arr[10][20];
	 
	     n=getLaserStrength(buf,arr);
		 //debug//printf("char array arr[1] = %s",arr[1]);
		 sscanf(arr[1], "%d", &LaserStrength);
		 //debug//printf("char array arr[1] to integer LaserStrength = %d\n",LaserStrength);
			if (ChoiceInverseDepth == 'y' || ChoiceInverseDepth == 'Y')
			{
			Z_depthRate = Z_retraction-((((255-LaserStrength)*100)/255)*(Z_retraction-Z_depth)/100);
			Z_depthRate_digit = abs(((Z_retraction_digit-Z_depth_digit)*100*Z_depthRate)/100);
			//debug//printf("Z-diepte-verhouding = %d.%d\n",Z_depthRate,Z_depthRate_digit);
			}
			else if (ChoiceInverseDepth == 'n' || ChoiceInverseDepth == 'N')
			{
			Z_depthRate = Z_retraction-(((LaserStrength*100)/255)*(Z_retraction-Z_depth)/100);
			Z_depthRate_digit = abs(((Z_retraction_digit-Z_depth_digit)*100*Z_depthRate)/100);
			//debug//printf("Z-diepte-verhouding = %d.%d\n",Z_depthRate,Z_depthRate_digit);
			}
		 //printf(";next line changed S into Z\n");
		 //fprintf(out_file, ";next line changed S into Z\n");
		 printf("%sZ%d.%d\n",arr[0],Z_depthRate,Z_depthRate_digit);
		 //debug//printf("%s\n",buf);
		 fprintf(out_file, "%sZ%d.%d\n",arr[0],Z_depthRate,Z_depthRate_digit);
         }
         else
         {
         printf("%s",buf);
         fprintf(out_file, "%s",buf);
         }
         fclose(in_file);
         fclose(out_file);
         printf("All files closed\n");	
   }
   else
   {
   printf("Failed to open the file\n");
   }
return(0);
}
