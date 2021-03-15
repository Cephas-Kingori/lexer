#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define MAX_SOURCE_FILE_SIZE 10000
#define MAX_IDENTIFIER_LEN 32

int main() {

   FILE * stream_ptr;
   stream_ptr = fopen("source.txt", "r");
   if (stream_ptr != NULL) {
      //read the source file into variable source character by character
      char source[MAX_SOURCE_FILE_SIZE];

      char c;
      size_t i = 0;

      while ((c = fgetc(stream_ptr)) != EOF) {
         source[i] = (char) c;
         i++;
      }
      source[i] = ' ';
      source[i + 1] = '\0';
      fclose(stream_ptr);

      printf("%s", source);
      printf("\n\n\n");

      /*recognize the lexemes*/
      //store temporarily the lexemes
      char temp[MAX_IDENTIFIER_LEN];
      //symbol table
      char symb[MAX_SOURCE_FILE_SIZE] = "\0";
      //conversion buffer for converting integers to strings
      char buffer[MAX_IDENTIFIER_LEN];
      //storage of errors
      char errors[MAX_SOURCE_FILE_SIZE] = "\0";

      size_t source_position = 0;
      int line_number = 1;
      size_t temp_index = 0;

      while ((c = source[source_position]) != '\0') {
         //string literals
         if (c == '"') {
            size_t j = 0;
            temp[j] = '"';
            j++;
            source_position++;
            while ((c = source[source_position]) != '"') {
               temp[j] = c;
               source_position++;
               j++;

               //find errors
               char peep;
               peep = source[source_position];
               if (peep == '\0') {
                  //if in the whole file you could not find another quote
                  //errors
                  strcat(errors, temp);
                  strcat(errors, "\t There was no matching quote for the string.\n");
                  memset(temp, 0, strlen(temp));
                  break;
               }
            }
            if (c == '"') {
               temp[j] = c;
               strcat(symb, "T_STR\t ");
               strcat(symb, temp);
               strcat(symb, "\n");
               memset(temp, 0, strlen(temp));
            }
         }
         //integer numeral
         else if (c >= 48 && c <= 57) {
            //if it starts with a number
            size_t j = 0;

            while (c >= 48 && c <= 57) {
               temp[j] = c;
               j++;
               //if the next is a number, continue, else, that is the end of numerals
               if ((source[source_position + 1]) >= 48 && (source[source_position + 1]) <= 57) {
                  source_position++;
                  c = source[source_position];
                  continue;
               } else {
                  //save the current position since we are just looking ahead for errors
                  int return_to = source_position;

                  source_position++;
                  c = source[source_position];
                  // if the next character after all the numerals is a valid operator like - or + or ; or space
                  if ((c == 32) || (c == 33) || (c == 38) || (c >= 41 && c <= 43) || (c == 45) || (c == 47) || (c >= 59 && c <= 60) || (c == 62) || (c == 93) || (c == 124)) {
                     strcat(symb, "T_INT\t ");
                     strcat(symb, temp);
                     strcat(symb, "\n");
                     memset(temp, 0, strlen(temp));
                     //return to the last digit
                     source_position = return_to;
                  } else {
                     //there was an error in what you were scanning
                     //until you find a valid character that should come after a numeral
                     //record it into errors
                     while (!((c == 32) || (c == 33) || (c == 38) || (c >= 41 && c <= 43) || (c == 45) || (c == 47) || (c >= 59 && c <= 60) || (c == 62) || (c == 93) || (c == 124))) {
                        temp[j] = c;
                        char peep;
                        peep = source[source_position + 1];
                        if ((peep == 32) || (peep == 33) || (peep == 38) || (peep >= 41 && peep <= 43) || (peep == 45) || (peep == 47) || (peep >= 59 && peep <= 60) || (peep == 62) || (peep == 93) || (peep == 124)) {
                           strcat(errors, temp);
                           strcat(errors, "\t there is an error in this numeral.\n");
                           memset(temp, 0, strlen(temp));
                           break;
                        } else {
                           j++;
                           source_position++;
                           c = source[source_position];
                        }
                     }

                  }
               }
            }
         }
         //identifier
         else if ((c >= 65 && c <= 90) || (c == 95) || (c >= 97 && c <= 122)) {
            //if alphanumeric until you find an operator
            size_t j = 0;
            char next;
            do {
               temp[j] = c;

               char peep;
               peep = source[source_position + 1];

               //if the next character is a legal character
               //that is allowed in an identifier(including a digit), continue

               if ((peep >= 48 && peep <= 57) || (peep >= 65 && peep <= 90) || (peep == 95) || (peep >= 97 && peep <= 122)) {
                  j++;
                  source_position++;
                  c = source[source_position];
                  continue;
               } else {
                  if ((peep == 32) || (peep == 33) || (peep == 38) || (peep >= 40 && peep <= 45) || (peep == 47) || (peep >= 59 && peep <= 62) || (peep == 91) || (peep == 93) || (peep == 123) || (peep == 124)) {
                     //if the next one is a legal operator
                     //and hence what you have seen is the full identifier

                     //now, separate the identifiers from the keywords
                     //check if it is a keyword
                     int a;
                     if ((a = strcmp(temp, "int")) == 0) {
                        strcat(symb, "TT_INT\t int\n");
                        /*By this point the lexeme is identified and placed into the symbol table*/
                        //clear the temp since you are about to see a new lexeme
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "else")) == 0) {
                        strcat(symb, "TT_ELSE\t else\n");
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "char")) == 0) {
                        strcat(symb, "TT_CHAR\t char\n");
                        /*By this point the lexeme is identified and placed into the symbol table*/
                        //clear the temp since you are about to see a new lexeme
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "void")) == 0) {
                        strcat(symb, "TT_VOID\t void\n");
                        memset(temp, 0, strlen(temp));
                        break;

                     } else
                     if ((a = strcmp(temp, "while")) == 0) {
                        strcat(symb, "T_WHILE\t while\n");
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "if")) == 0) {
                        strcat(symb, "T_IF\t if\n");
                        memset(temp, 0, strlen(temp));
                        break;

                     } else
                     if ((a = strcmp(temp, "for")) == 0) {
                        strcat(symb, "T_FOR\t for\n");
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "print")) == 0) {
                        strcat(symb, "T_PRINT\t print\n");
                        memset(temp, 0, strlen(temp));
                        break;
                     } else
                     if ((a = strcmp(temp, "return")) == 0) {
                        strcat(symb, "T_RETURN\t return\n");
                        memset(temp, 0, strlen(temp));
                        break;
                     }
                     //if it was none of the above, then it was a simple identifier
                     strcat(symb, "T_ID\t ");
                     strcat(symb, temp);
                     strcat(symb, "\n");
                     memset(temp, 0, strlen(temp));
                     break;

                  } else {
                     //the next is not alphanumeric and is not an operator
                     //that is probably an error
                     j++;
                     source_position++;
                     c = source[source_position];
                     while (!((c == 32) || (c == 33) || (c == 38) || (c >= 40 && c <= 45) || (c == 47) || (c >= 59 && c <= 62) || (c == 91) || (c == 93) || (c == 123) || (c == 124) || (c == '\0'))) {
                        //save the erroneous identifier until you find a valid delimiter or reach the end of file
                        temp[j] = c;
                        j++;
                        source_position++;
                        c = source[source_position];
                     }
                     source_position--;
                     strcat(errors, temp);
                     strcat(errors, "\t is not a correct identifier.\n");
                  }
               }
            } while (!((c == 32) || (c == 33) || (c == 38) || (c >= 40 && c <= 45) || (c == 47) || (c >= 59 && c <= 62) || (c == 91) || (c == 93) || (c == 123) || (c == 124)));

         } else {
            //if it was not a literal
            //not an integer literal
            //not a keyword
            //not an identifier
            //check if it is a valid operator
            switch (c) {
            case '/':
               if (source[source_position + 1] == '/') {
                  //single line comment
                  //repeat until newline feed is reached or break if file ends
                  do {
                     source_position++;
                     c = source[source_position];
                     if (c == '\0') {
                        //if you reach the end of file
                        //e.g. when comment goes on to the end of file
                        break;
                     }

                  } while (c != 10);

               } else if (source[source_position + 1] == '*') {
                  source_position++;
                  c = source[source_position];
                  source_position++;
                  char peep;
                  while (c) {
                     c = source[source_position];
                     peep = source[source_position + 1];
                     if (c == '*') {
                        if (peep == '/') {
                           source_position++;
                           break;
                        }
                     }
                     if (c == '\0') {
                        break;
                     }
                     source_position++;
                  }
               } else {
                  //just a division sign
                  strcat(symb, "T_BIN_DIV\t /\n");
               }
               break;
            case ' ':
               //do nothing, only see and move on.
               //ignores white spaces
               break;
            case 10:
               //do nothing, only see and move on.
               //ignores newline feeds
               break;
            case '+':
               if (source[source_position + 1] == '+') {
                  strcat(symb, "T_INC\t ++\n");
                  source_position++;
               } else
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_ASSIGN_INC\t +=\n");
                  source_position++;
               } else {
                  strcat(symb, "T_BIN_ADD\t +\n");
               }
               break;
            case '=':
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_EQUALITY\t ==\n");
                  source_position++;
               } else {
                  strcat(symb, "T_ASSIGN\t =\n");
               }
               break;
            case '-':
               if (source[source_position + 1] == '-') {
                  strcat(symb, "T_DEC\t --\n");
                  source_position++;
               } else
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_ASSIGN_DEC\t -=\n");
                  source_position++;
               } else {
                  strcat(symb, "T_MINUS\t -\n");
               }
               break;
            case '!':
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_INEQUALITY\t !=\n");
                  source_position++;
               } else {
                  strcat(symb, "T_NOT\t !\n");
               }
               break;
            case '{':
               strcat(symb, "T_LBRACE\t {\n");
               break;
            case '}':
               strcat(symb, "T_RBRACE\t }\n");
               break;
            case '(':
               strcat(symb, "T_LBRACKET\t (\n");
               break;
            case ')':
               strcat(symb, "T_RBRACKET\t )\n");
               break;
            case '*':
               strcat(symb, "T_MUL\t *\n");
               break;
            case '>':
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_GREQ\t >=\n");
                  source_position++;
               } else {
                  strcat(symb, "T_GRE\t >\n");
               }
               break;

            case '<':
               if (source[source_position + 1] == '=') {
                  strcat(symb, "T_LESSEQ\t <=\n");
                  source_position++;
               } else {
                  strcat(symb, "T_LESS\t <\n");
               }
               break;
            case '&':
               if (source[source_position + 1] == '&') {
                  strcat(symb, "T_AND\t &&\n");
                  source_position++;
               } else {
                  /////RAISE ERROR
                  temp[0] = c;
                  source_position++;
                  c = source[source_position];
                  size_t j = 1;
                  while (!((c == 32) || (c >= 40 && c <= 41) || (c >= 48 && c <= 57) || (c == 59) || (c >= 65 && c <= 90) || (c == 95) || (c >= 97 && c <= 122) || (c == 123) || (c == 125) || (c == '\0'))) {
                     //save the erroneous identifier until you find a valid delimiter or reach the end of file
                     temp[j] = c;
                     j++;
                     source_position++;
                     c = source[source_position];
                  }
                  source_position--;
                  strcat(errors, temp);
                  strcat(errors, "\t error identifying operator. Only && is allowed.\n");
               }
               break;
            case '|':
               if (source[source_position + 1] == '|') {
                  strcat(symb, "T_OR\t ||\n");
                  source_position++;
               } else {
                  /////RAISE ERROR
                  temp[0] = c;
                  source_position++;
                  c = source[source_position];
                  size_t j = 1;
                  while (!((c == 32) || (c >= 40 && c <= 41) || (c >= 48 && c <= 57) || (c == 59) || (c >= 65 && c <= 90) || (c == 95) || (c >= 97 && c <= 122) || (c == 123) || (c == 125) || (c == '\0'))) {
                     //save the erroneous identifier until you find a valid delimiter or reach the end of file
                     temp[j] = c;
                     j++;
                     source_position++;
                     c = source[source_position];
                  }
                  source_position--;
                  strcat(errors, temp);
                  strcat(errors, "\t error identifying operator. Only || is allowed.\n");

               }
               break;
            case ';':
               strcat(symb, "T_SEMICOLON\t ;\n");
               break;
            case ',':
               strcat(symb, "T_COMMA\t ,\n");
               break;
            default:
               //that character is not allowed
               ;
               //the semicolon above is important

               printf("errrrrrrr %d\n", c);

               size_t j = 0;
               while (!((c == 32) || (c >= 40 && c <= 41) || (c >= 48 && c <= 57) || (c == 59) || (c >= 65 && c <= 90) || (c == 95) || (c >= 97 && c <= 122) || (c == 123) || (c == 125) || (c == '\0'))) {
                  //save the erroneous identifier until you find a valid delimiter or reach the end of file
                  temp[j] = c;
                  j++;
                  source_position++;
                  c = source[source_position];
               }
               source_position--;
               strcat(errors, temp);
               strcat(errors, "\t error identifying operator.\n");
            }
         }
         source_position++;
      }
      //print the symbol table
      printf("%s", symb);
      printf("\n\n\n");
      printf("%s", errors);
   } else {
      printf("Error opening file");
   }

   return 0;
}
