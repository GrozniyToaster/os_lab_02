#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAIL -1
#define BUF_SIZE 256
#define MAX_STRING  256
#define NUMERAL_SYSTEM 10


int IsSep( const char ch ){
    return ch == ' ' || ch == '\n' || ch == '\t';
}
int IsEndStr( const char ch ){
    return ch == '\0' || ch == EOF;
}
int IsNum( const char ch ){
    return '0'<= ch && ch <= '9';
}

int  StrToFloatTok( const char* str, float* floatTok, int countTok ){ //разбиение строки на flot'ы
    int i = 0;
    for ( int tok = 0; tok < countTok; ++tok ){
        int sign = 1;
        int intPart = 0;
        while( IsSep(str[i]) ){
            ++i;
        }
        if ( str[i] == '-' ){   //берем знак если минус
            sign = -1;
            ++i;
        }
        while( IsSep(str[i]) ){
            ++i;
        }
        while( IsNum( str[i] ) ){      //дошли до целой части
            intPart = ( intPart * NUMERAL_SYSTEM ) + ( str[i] - '0' );
            ++i;
        }
        if ( IsSep(str[i]) ){       // проверяем что прервало число запятая или пробел и тд.
            floatTok[tok] = sign * intPart;
            continue;
        }
        if ( IsEndStr(str[i]) ){
            floatTok[tok] = sign * intPart;
            return tok + 1;
        }
        ++i;
        float floatPart = 0;    //считываем число после запятой
        int floatPartSize = 1;
        while( IsNum(str[i]) ){
            floatPart = floatPart + ( ( str[i] - '0' ) * pow( NUMERAL_SYSTEM , -1 *  floatPartSize ) );
            ++floatPartSize;
            ++i;
        }
        floatTok[tok] = sign * (intPart + floatPart);
        if ( IsEndStr(str[i]) ){
            return tok + 1;
        }
    }
    return countTok;
}

int ReadString( int fd, char* str ){    // чтение строки с потока
    char ch;
    int readCh = 0;

    if ( read( fd, &ch, sizeof( char ) ) < 1 ){
        return EOF;
    }
    while ( IsSep(ch) ){
        if ( read( fd, &ch, sizeof( char ) ) < 1){
            return EOF;
        }
    }

    while( readCh < MAX_STRING ){
        str[readCh] = ch;
        if ( read( fd, &ch, sizeof( char ) ) < 1 ){
            break;
        }
        if ( ch == '\n' ){
            break;
        }
        ++readCh;
    }

    ++readCh;
    if ( readCh >= MAX_STRING ){
        perror("Oversize string");
        exit( FAIL );
    }
    str[readCh] = '\0';

    return readCh;
}

int ChildWork(){
    float floats[3];
    char command[BUF_SIZE];
    while( ReadString( STDIN_FILENO, command ) > 0 ){
        if ( StrToFloatTok( command, floats, 3 ) < 3 ){
            perror("Nonvalid command");
            perror(command);
            return FAIL;
        }
        for( int i = 1; i < 3; ++i ){
            float res = floats[0] / floats[i];
            if ( isinff( res ) ){
                perror("Division by zero");
                return FAIL;
            }
            write( STDOUT_FILENO, &res, sizeof( float )  );
        }
    }
    return SUCCESS;
}

void ParentWork( int childFD ){
    float toPrint;
    char pr[BUF_SIZE];
    while( read( childFD, &toPrint, sizeof( float ) ) > 0 ){
        sprintf( pr,"Received from child %4.4f \n", toPrint );
        write( STDOUT_FILENO, pr , strlen( pr ) * sizeof( char ) );
    }
}

int main(){
    int fd[2], pipe1[2];

    if ( pipe(fd) != 0 ){
        return FAIL;
    }
    if ( pipe( pipe1 ) != 0 ){
        return FAIL;
    }

    int id = fork();
    if ( id < 0 ){
        perror("Fork error");
		return FAIL;

    }else if( id == 0){ // Программа ребенка
        
        close( fd[1] );
        close( pipe1[0] );
        fflush(stdout);
        
        char fileName[BUF_SIZE];
        ReadString( fd[0], fileName );
        
        int  file = open( fileName, O_RDONLY );
        if ( file < 0 ){
            perror("Child can't open file");
            exit( FAIL );
        }

        close( fd[0] );
        close( STDIN_FILENO );
        dup2( file, STDIN_FILENO ); // перенаправляем стандартный поток ввода-вывода
        close( STDOUT_FILENO );
        dup2( pipe1[1], STDOUT_FILENO );

        if ( ChildWork() < 0 ){
            close( pipe1[1] );
            exit( FAIL );
        }

        close( pipe1[1] );
        exit( SUCCESS );

    }else{ //Программа родителя
        
        close( pipe1[1] ); // закрываем неиспользуемые дескрипторы конвееров
        close( fd[0] ); 
        fflush(stdout);    // и сбрасываем буффер вывода

        char fileName[BUF_SIZE];
        if ( ReadString( STDIN_FILENO, fileName ) < 1 ){
            perror("Need file with commands");
            exit( FAIL );
        }

        int lenName = strlen( fileName );
        write( fd[1], fileName, sizeof( char ) * lenName );
        close( fd[1] );

        ParentWork( pipe1[0] );
        
        close( pipe1[0] );
        exit( SUCCESS );
    }
}