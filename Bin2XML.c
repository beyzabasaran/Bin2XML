#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <wchar.h>
#include <locale.h>

typedef struct{   // struct size=252 byte
 char name[64];   // utf16
 char surname[32];   // utf8
 char gender;  // print with %c
 char email[32];
 char phone_number[16];
 char address[32];
 char level_of_education[8]; 
 unsigned int income_level;   // given little-endian
 unsigned int expenditure;  // given big-endian
 char currency_unit[16];
 char currentMood[32];
 float height;    // print with %f-> %.2f
 unsigned int weight;   // print with %u
}record;

typedef struct{   // struct size=168 byte
 char name_tag[8];  
 char surname_tag[8];   // surname=7 length->8 
 char gender_tag[8];
 char email_tag[8];
 char phone_number_tag[16];   // phone_number=12 length->16 
 char address_tag[8];
 char level_of_education_tag[32];   // level_of_education=18 length->32 
 char income_level_tag[16];
 char expenditure_tag[16];
 char currency_unit_tag[16];
 char currentMood_tag[16];
 char height_tag[8];
 char weight_tag[8];
}header_struct;

unsigned int swap_Endians(unsigned int value);    // Declaring the function

int main(int argc, char *argv[]){ 
  FILE *inputfile;
  FILE *outputfile;
  FILE *inputfile_second_pointer;
  record person,temp,UTF16_person;   // Declaring variable 
  header_struct header;
  int tag_flag=1;   // flag for assigning first read to the header tags
  int row_id=0;    // keeping row id

  
  char ch1;   // first character
  char ch2;   // second character
  char UFT16_name[sizeof(person.name)/2];    
  char UTF16_person_rest[sizeof(record)-sizeof(person.name)];  //for reading rest attributes of struct record

  //printf("Struct Size: %d\n",sizeof(record)); //252
  //printf("Struct Size: %d\n",sizeof(header_struct)); //168

  /*Command Line Arguments Check*/
  if(argc < 3){
    printf("Error: Missing argument!\n");
    printf("Example Usage: ./Bin2XML <records.dat> <records.xml>\n");
    return 0;
  }

  if (strcmp(argv[0],"./Bin2XML")!=0) {
    printf("Error: Program name must be Bin2XML!\n");
    printf("Example Usage: ./Bin2XML <records.dat> <records.xml>\n");
    return 0;
  }
  if (strcmp(argv[1],"records.dat")!=0) {
    printf("Error: Input file must be records.dat!\n");
    printf("Example Usage: ./Bin2XML <records.dat> <records.xml>\n");
    return 0;
  }
  if (strcmp(argv[2],"records.xml")!=0) {
    printf("Error: Output XML file must have same name with the root element records!\n");
    printf("Example Usage: ./Bin2XML <records.dat> <records.xml>\n");
    return 0;
  }

  inputfile = fopen (argv[1],"r");
  outputfile = fopen (argv[2],"w");
  inputfile_second_pointer = fopen (argv[1],"r");  //second pointer for reading names in the UTF-16 format
  
  /*File Check*/
  if (inputfile == NULL){
    printf("Error opening file!\n");
    exit (1);
  }
    
  /*Reading record struct size of information and assigning person and header tag attributes*/  
  while (fread (&person, sizeof(record), 1, inputfile)){
    if(tag_flag==1){
      temp=person;
      printf("\n%s %s %c %s %s %s %s %u %u %s\n%s %.2f %u\n",temp.name,temp.surname,temp.gender,
      temp.email,temp.phone_number,temp.address,temp.level_of_education,
      temp.income_level,temp.expenditure,temp.currency_unit,temp.currentMood,
      temp.height,temp.weight);
      
      /*Assigning the tag names*/
      if(strcmp(temp.name, "name")==0){
        strcpy(header.name_tag, "name");
      }
      if(strcmp(temp.surname, "surname")==0){
        strcpy(header.surname_tag, "surname");
      }
      if(temp.gender == 'g'){
        strcpy(header.gender_tag, "gender");
      }
      if(strcmp(temp.email, "email")==0){
        strcpy(header.email_tag, "email");
      }
      if(strcmp(temp.phone_number, "phone_number")==0){
        strcpy(header.phone_number_tag, "phone_number");
      }
      if(strcmp(temp.address, "address")==0){
        strcpy(header.address_tag, "address");
      }
      if(strcmp(temp.level_of_education, "level_of_education")!=0){
        strcpy(header.level_of_education_tag, "level_of_education");
      }
      if(!temp.income_level>0){
        strcpy(header.income_level_tag, "income_level");
      }
      if(!temp.expenditure>0){
        strcpy(header.expenditure_tag, "expenditure");
      }
      if(strcmp(temp.currency_unit,"currency_unit")==0){
        strcpy(header.currency_unit_tag,"currency_unit");
      }
      if(strcmp(temp.currentMood, "currentMood")==0){
        strcpy(header.currentMood_tag, "currentMood");
      }
      if(!temp.height>0){
        strcpy(header.height_tag, "height");
      }
      if(!temp.weight>0){
        strcpy(header.weight_tag, "weight");
      }
      printf("\n%s %s %s %s %s %s %s %s %s %s \n%s %s %s\n",header.name_tag,header.surname_tag,
      header.gender_tag,header.email_tag,header.phone_number_tag,header.address_tag,
      header.level_of_education_tag,header.income_level_tag, header.expenditure_tag,
      header.currency_unit_tag,header.currentMood_tag,header.height_tag,header.weight_tag);
      puts("Tag written...\n");
      tag_flag=0;//flag false
      fread (&UTF16_person, sizeof(record), 1, inputfile_second_pointer);
      // reading first size of record to have names on the next read for UTF-16 name conversion
      continue;
    }
    
    if(row_id==0){
      /*Starting to write XML file*/
      fprintf(outputfile,"%s","<?xml version=\"1.0\"?>\n"); //" encoding=\"utf-8
      fprintf(outputfile,"<records>\n");
    }

    if(strlen(person.name)!= 0 && tag_flag!=1){   // Check if there exists a valid person
      //UTF-16 Conversion Trials 
      for(int i=0;i< sizeof(person.name)/2;i++){  //half length of person.name
        fread (&ch1, 1, 1, inputfile_second_pointer);  // read first char
        UFT16_name[i]= ch1;
        fread (&ch2, 1, 1, inputfile_second_pointer);  // read second char
        UFT16_name[i]+= ch2;   //add first and second char
      }
      printf("UTF16 name:%s\n",UFT16_name);
      fread (&UTF16_person_rest, sizeof(UTF16_person_rest), 1, inputfile_second_pointer); //read the rest 
      /*int length=strlen(person.name);
      char temp_name[length];
      strcpy(temp_name,person.name);
      for(int i=0; i<length;i++){
        printf("%02x ",temp_name[i]);
      }
      printf("\n");*/  
      int str_size = strlen(person.name);
      char str[str_size];
      strcpy(str,person.name);   
      printf("Separed by 2 characters: ");  
      for (int i = 0; i< str_size; i++)
      {
        if (i % 2== 0 && i!=0){
          printf("/");   //separating 2 character by 2 character
        }
        printf("%c ", str[i]);   
      }
      printf("\n");
  
      printf("name:%s surname:%s gender:%c email:%s phone_number:%s\n" 
      "address:%s level_of_education:%s income_level:%u expenditure:%u currency_unit:%s\n" 
      "currentMood:%s height:%.2f weight:%u\n",person.name,person.surname,person.gender,
      person.email,person.phone_number,person.address,person.level_of_education,
      person.income_level,person.expenditure,person.currency_unit,person.currentMood,
      person.height,person.weight); 

      /*BigEnd and LittleEnd Conversions*/
      unsigned int littleEnd_income_level;
      unsigned int bigEnd_income_level;
      unsigned int bigEnd_expenditure;
      unsigned int littleEnd_expenditure;
    
      char *c1 = (char*)&person.income_level; 
      char *c2 = (char*)&person.expenditure; 

      if (*c1){//BigEndian-LittleEndian check 
        printf("\nincome_level-> Little endian\n");
        littleEnd_income_level = person.income_level;
        //bigEnd_income_level = __builtin_bswap32(littleEnd_income_level);  //built in func. to reverse bytes 
        bigEnd_income_level =swap_Endians(littleEnd_income_level);  //implemented function
        printf("Little-endian: %u\n", littleEnd_income_level);
        printf("Big-endian: %u\n", bigEnd_income_level);
      }  
      else{
        printf("\nincome_level-> Big endian");       
      }
      if (*c2){
        printf("\nexpenditure -> Little endian");
      }  
      else{
        printf("\nexpenditure-> Big endian\n");
        bigEnd_expenditure = person.expenditure;
        //littleEnd_expenditure = __builtin_bswap32(bigEnd_expenditure);
        littleEnd_expenditure = swap_Endians(bigEnd_expenditure);   //swapping endians
        printf("Big-endian: %u\n", bigEnd_expenditure);
        printf("Little-endian: %u\n", littleEnd_expenditure);
        person.expenditure= littleEnd_expenditure;    //assigning new endian value
      }  

      /*Continuing to write XML file*/
      fprintf(outputfile,"    ""<row row_id= \"%d\">\n",++row_id);
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.name_tag,person.name,header.name_tag);  
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.surname_tag,person.surname,header.surname_tag); 
      fprintf(outputfile,"        ""<%s>%c</%s>\n",header.gender_tag,person.gender,header.gender_tag);  
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.email_tag,person.email,header.email_tag); 
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.phone_number_tag,person.phone_number,header.phone_number_tag); 
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.address_tag,person.address,header.address_tag); 
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.level_of_education_tag,person.level_of_education,header.level_of_education_tag);
      fprintf(outputfile,"        ""<%s bigEnd=\"%u\">%u</%s>\n",header.income_level_tag,bigEnd_income_level,person.income_level,header.income_level_tag); 
      fprintf(outputfile,"        ""<%s bigEnd=\"%u\">%u</%s>\n",header.expenditure_tag,bigEnd_expenditure,person.expenditure,header.expenditure_tag); 
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.currency_unit_tag,person.currency_unit,header.currency_unit_tag); 
      fprintf(outputfile,"        ""<%s>%s</%s>\n",header.currentMood_tag,person.currentMood,header.currentMood_tag); 
      fprintf(outputfile,"        ""<%s>%.2f</%s>\n",header.height_tag,person.height,header.height_tag); 
      fprintf(outputfile,"        ""<%s>%u</%s>\n",header.weight_tag,person.weight,header.weight_tag); 
      fprintf(outputfile,"    ""</row>\n");
       
      puts("\nRecord written...\n");     
    }
    
  }
  fprintf(outputfile,"</records>\n");
   /*Closing the file*/
  fclose(inputfile);
  fclose(outputfile);
  return 0;//END
    
}

/*Swap Endians Implemented From: 
https://www.geeksforgeeks.org/bit-manipulation-swap-endianness-of-a-number/*/
unsigned int swap_Endians(unsigned int value) 
{ 
  unsigned int result; 
  unsigned int leftmost_byte; //holding leftmost 8 bits of the result. 
  unsigned int left_middle_byle; //holding left middle 8 bits of the result 
  unsigned int right_middle_byte; //holding right middle 8 bits of the result 
  unsigned int rightmost_byte; //holding rightmost 8 bits of the result 
  
  //Getting rightmost 8 bits of the number by anding it with 0x000000FF. 
  //Since the last 8 bits are all ones, the result will be the rightmost 8 bits of the number. 
  //this will be converted into the leftmost 8 bits for the output (swapping) 
  leftmost_byte = (value & 0x000000FF) >> 0; 
  
  // Getting the right middle and left middle 8 bits which will become 
  // the left_middle bits in the output 
  left_middle_byle = (value & 0x0000FF00) >> 8; 
  right_middle_byte = (value & 0x00FF0000) >> 16; 
  
  // Getting the leftmost 8 bits which will be the rightmost 8 bits of the output 
  rightmost_byte = (value & 0xFF000000) >> 24; 
  
  //Shifting 8 bits to left by 24 so that it becames shifted to the leftmost end 
  leftmost_byte <<= 24; 
  
  // Same as above, left shifting by 16 so that it is in the left_middle 
  // position.
  left_middle_byle <<= 16; //it ends at the 16th bit from the left 
  right_middle_byte <<= 8; //and starts at the 9th bit from the left

  rightmost_byte <<= 0;  // The rightmost bit stays as it is since it is in the correct position

  //Finally, concatenating all of the values and returning the result
  result = (leftmost_byte | left_middle_byle |right_middle_byte | rightmost_byte); 
  return result; 
} 


