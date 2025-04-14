#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

enum type_of_lex {
    LEX_NULL, LEX_AND, LEX_BOOL, LEX_ELSE, LEX_GOTO, LEX_IF, LEX_FALSE,
//  "",       and,     boolean,  else,     goto,     if,     false,
    
    LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_STRING, LEX_TRUE, 
//  int,     not,     or,     program,     read,     string,     true,

    LEX_WHILE, LEX_WRITE, 
//  while,     write     

    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAR, LEX_RPAR, LEX_LBRACKET, LEX_RBRACKET, LEX_ASSIGN, LEX_EQUAL, 
//  ;              ,          :          (            )      {             }             =           == 
    LEX_PLUS, LEX_MINUS, LEX_MULT, LEX_DIV, LEX_MOD, LEX_LESS, LEX_GREATER, LEX_LEQ, LEX_GEQ, LEX_NEQ, 
//  +         -          *         /        %        <         >            <=       >=       !=        
     
    LEX_NUM,
    LEX_ID, 
    LEX_WORD,
    LEX_FIN,
    POLIZ_LABEL, 
    POLIZ_ADDRESS, 
    POLIZ_GO, 
    POLIZ_FGO};
    


    class Lex { // делаем пары - лексема<->тип лексемы
        friend class Scanner;
        type_of_lex t_lex;
        int v_lex;
        public:
        Lex ( type_of_lex t = LEX_NULL, int v = 0) {
            t_lex = t; v_lex = v;
        }
        type_of_lex get_type()const { return t_lex; }
        int get_value()const { return v_lex; }
        friend ostream& operator << (ostream &s, Lex l);
    };

class Ident { // описание переменных и констант
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    public:
    Ident() { 
        declare = false; assign = false; 
    }
    Ident(string n) {
        name=n; declare = false; assign = false; 
    }
    
    bool operator== (const string& s) const {
        return name == s; 
    }
    string get_name() { return name; }
    bool get_declare() { return declare; }
    void set_declare() { declare = true; }
    type_of_lex get_type() { return type; }
    void set_type(type_of_lex t) { type = t; }
    bool get_assign() { return assign; }
    void set_assign(){ assign = true; }
    int get_value() { return value; }
    void set_value(int v){ value = v; }
};

vector <Ident> TID; //таблица переменных и констант
int put (const string & buf) { // либо определяем, что переменная уже была
                               // либо помещаем в таблицу новую
    vector <Ident>::iterator k;
    if ( (k = find(TID.begin(), TID.end(), buf)) != TID.end()) {
        int index = distance(TID.begin(), k);
        return index;
    }
    TID.push_back ( Ident(buf) );
    return TID.size() - 1;
}

class Scanner { //сканер файла
    friend class Lex;
    char c;
    int look (const string& buf, const char ** list){
        int i = 0;
        while (list [i]) {
            if ( buf == list [i] ) 
            return i;
            i++;
        }
        return 0;
    }
    void gc() { c = fgetc(fp); 
        }
    public:
		static int str_counter;
        static FILE * fp;
        static const char * TW[],* TD[];
        Scanner (const char * program){
            fp = fopen( program, "r" );
            if (fp==NULL) throw "can't open file";
        }
        Lex get_lex();
    };
    FILE* Scanner::fp = NULL;
    
    const char* Scanner:: TW [] = { //таблица служебных слов
    
    "", "and","boolean","else", "goto", "if", "false",
//  0   1     2         3       4       5     6
    "int","not","or","program","read","string","true",
//  7     8     9    10        11      12      13
    "while","write", NULL
//  14      15
    };
    const char* Scanner:: TD [] = { //таблица разделителей
    "\"", ";", ",", ":", "(", ")", "{", "}", "=", "==", 
//  0     1    2    3    4    5    6    7    8    9
    "+", "-", "*", "/", "%", "<", ">", "<=", "!=", ">=", NULL
//  10   11   12   13   14   15   16   17    18    19
    };

int Scanner::str_counter = 1;

ostream& operator << (ostream &s, Lex l)
    {       
		vector<string> types{"LEX_NULL", "LEX_AND", "LEX_BOOL", "LEX_ELSE", "LEX_GOTO", "LEX_IF", "LEX_FALSE",
//  "",       and,     boolean,  else,     goto,     if,     false,
    
    "LEX_INT", "LEX_NOT", "LEX_OR", "LEX_PROGRAM", "LEX_READ", "LEX_STRING", "LEX_TRUE", 
//  int,     not,     or,     program,     read,     string,     true,

    "LEX_WHILE", "LEX_WRITE", 
//  while,     write     

    "LEX_SEMICOLON", "LEX_COMMA", "LEX_COLON", "LEX_LPAR", "LEX_RPAR", "LEX_LBRACKET", "LEX_RBRACKET", "LEX_ASSIGN", "LEX_EQUAL", 
//  ;              ,          :          (            )      {             }             =           == 
    "LEX_PLUS", "LEX_MINUS", "LEX_MULT", "LEX_DIV", "LEX_MOD", "LEX_LESS", "LEX_GREATER", "LEX_LEQ", "LEX_GEQ", "LEX_NEQ", 
//  +         -          *         /        %        <         >            <=       >=       !=        
     
    "LEX_NUM",
    "LEX_ID", 
    "LEX_WORD",
    "LEX_FIN",
    "POLIZ_LABEL", 
    "POLIZ_ADDRESS", 
    "POLIZ_GO", 
    "POLIZ_FGO"}; 
        s << "( " << types[l.t_lex] << " , " << l.v_lex << " );";
        return s;
    }



Lex Scanner:: get_lex (){
    enum state{H,IDENT, NUMB, COM, ALE, STR, NEQ };
    state CS = H; string buf; int d, j;
    
    do { 
        gc();
        switch(CS) {
        case H:
            if(c ==' ' || c =='\n' || c=='\r' || c =='\t'){
				if ( c == '\n') str_counter++;
			}
            else
            if(isalpha(c)) {buf.push_back(c); CS = IDENT;}
            else
            if ( isdigit(c) ) { d = c - '0'; CS = NUMB; }
            else
            if ( c== '/' ) { 
				//cout << "here" << endl;
                gc();
                if ( c == '*') {CS = COM;} 
                else {
                    buf.push_back(c);
                    ungetc(c, fp);
                    j = look (buf, TD);
                    buf.clear();
                    return Lex((type_of_lex) (j+(int) LEX_SEMICOLON),j); // если нет
				}
            }
            else
            if (c== '=' || c== '<' || c== '>') {
                buf.push_back(c); CS = ALE; 
            }
            else
            if (c=='"'){ CS = STR; }
            else
            if (c == '!') {
                buf.push_back(c); CS = NEQ; 
            }
            else 
            if (c == ';' || c == ',' || c == ':' || c == '(' || c == ')' ||
                c == '{' || c == '}' || c == '+' || c == '-' || c == '*' || c == '%'){ 
                buf.push_back(c);
                if ( (j = look ( buf, TD)) )
                    buf.clear();
                    return Lex((type_of_lex) (j+(int) LEX_WRITE),j);
            }
            else
            if (feof(fp)){
                //cout << 1;
                return Lex(LEX_FIN);
            }
            else {
                    throw c;       
            }
            break;
        
        case IDENT:
            if ( isalpha(c) || isdigit(c)) {buf.push_back(c);} 
            else{ 
                ungetc(c, fp);
                CS = H;
                if ( j = look (buf, TW) ) {//проверка на служебное слово
                    buf.clear();
                    return Lex ((type_of_lex) j, j);
                }
                else {
                    j = put (buf); // не нашли -> название переменной
                    buf.clear();
                    return Lex (LEX_ID, j);
                }
            }
            break;
        
        case NUMB:
            if ( isdigit(c) ) {d = d * 10 + (c - '0');}
			else if (isalpha(c) ){
				throw "!Ошибка при лексическом анализе! Некорректный идентификатор";
			}
            else { 
                ungetc(c,fp); 
                CS = H;
                return Lex (LEX_NUM, d);
            }
            break;

        case COM:
            if ( c == '*' ) {
                gc();
                if (c == '/'){
                    CS = H;
                }
                else{
                    ungetc(c, fp);
                }
            }
            else
            if (feof(fp)) throw "!Ошибка при лексическом анализе! Незакрытый комментарий"; 
            break;

        case ALE:
            if (c=='='){ 
                buf.push_back(c); j = look( buf, TD);
                buf.clear(); CS=H;
                return Lex ((type_of_lex)(j+(int) LEX_SEMICOLON),j);
            }
            else {
                ungetc(c, fp); j = look (buf, TD); 
                buf.clear(); CS = H;
                return Lex((type_of_lex) (j+(int) LEX_SEMICOLON),j); 
            }
            break;

        case NEQ: 
            if (c == '=') {
                buf.push_back(c); j = look ( buf, TD );
                buf.clear(); CS = H;
                return Lex ( LEX_NEQ, j ); 
            }
            else throw "!Ошибка при лексическом анализе! Отдельный восклицательный знак";
        case STR:
            if (c != '"'){
                buf.push_back(c);
                if (feof(fp)){throw "!Ошибка при лексическом анализе! Незакрытая строка";}
            }
            else {
                CS=H; 
                int size = buf.size(); 
                buf.clear(); 
                return Lex(LEX_WORD, size);
            }

            break;
        } //end of switch
    } while (true);
} // end of getlex()



int main(int argc, char *argv[]){
    if (argc < 2) {
        cout << "Select %filename%.cpp as second argument" << endl;
        return 1;
    }
    
    try
    {   
        Scanner Lex_Scan(argv[1]);
        while (!feof(Scanner::fp)){
            cout << Lex_Scan.get_lex() << endl;
        }
        
    }
    catch(const char* message)
    {
		
        cerr << Scanner::str_counter <<  " : " << message << endl;
    }
    catch(string message)
    {
        cerr << Scanner::str_counter <<  " : " << message << endl;
    }
    catch(char c)
    {
        cerr << Scanner::str_counter <<  " : " << "!Ошибка при лексическом анализе! Недопустимый символ: " << c << endl;
    }

    return 0;
}
