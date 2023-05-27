/* 01.07.2020 */
#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
 
using namespace std;
 
enum type_of_lex {
    LEX_NULL,                                                                                   /* 0*/
    LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,        /* 9*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE,  /*18*/
    LEX_FIN,                                                                                    /*19*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,   /*27*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              /*35*/
    LEX_NUM,                                                                                    /*36*/
    LEX_ID,                                                                                     /*37*/
    POLIZ_LABEL,                                                                                /*38*/
    POLIZ_ADDRESS,                                                                              /*39*/
    POLIZ_GO,                                                                                   /*40*/
    POLIZ_FGO,                                                                                   /*41*/
    LEX_CONT,
    LEX_BRK,
    LEX_GT,
    LEX_LABEL                                                                        /*42*/
};
 
/////////////////////////  Класс Lex  //////////////////////////
 
class Lex {
    type_of_lex   t_lex;
    int           v_lex;
public:
    Lex ( type_of_lex t = LEX_NULL, int v = 0 ): t_lex (t), v_lex (v)  { }
    type_of_lex  get_type () const { 
    	  return t_lex; 
    }
    int get_value () const { 
    	  return v_lex; 
    }
  friend ostream & operator<< ( ostream &s, Lex l );
};
 
/////////////////////  Класс Ident  ////////////////////////////
 
class Ident {
    string      name;
    bool        declare;
    type_of_lex type;
    bool        assign;
    int          value;
public:
    Ident() { 
        declare = false; 
        assign  = false; 
    }
    bool operator== ( const string& s ) const { 
        return name == s; 
    }
    Ident ( const string n ) {
        name    = n;
        declare = false; 
        assign  = false;
    }
    string get_name () const { 
      return name; 
    }
    bool get_declare () const { 
      return declare; 
    }
    void put_declare () { 
      declare   = true; 
    }
    type_of_lex get_type () const { 
      return type; 
    }
    void put_type ( type_of_lex t ) { 
      type      = t; 
    }
    bool get_assign () const { 
      return assign; 
    }
    void put_assign () { 
      assign    = true; 
    }
    int  get_value () const { 
      return value; 
    }
    void put_value ( int v ) { 
      value     = v; 
    }
};
 
//////////////////////  TID  ///////////////////////
 
vector<Ident> TID;
 
int put ( const string & buf ) {
    vector<Ident>::iterator k;
 
    if ( ( k = find ( TID.begin (), TID.end (), buf ) ) != TID.end () )
        return k - TID.begin();
    TID.push_back ( Ident(buf) );
    return TID.size () - 1;
}
 
/////////////////////////////////////////////////////////////////



class Scanner {
    static FILE * fp;
    static char   c;
    static int look ( const string buf, const char ** list ) {
        int i = 0;
        while ( list[i] ) {
            if ( buf == list[i] )
                return i;
            ++i;
        }
        return 0;
    }
    static void gc () {
        c = fgetc (fp);
    }
public:
    static const char * TW [], * TD [], * CBG [];
    Scanner ( const char * program ) {
        if ( !(fp = fopen ( program, "r" )) ) 
            throw  "can’t open file" ;
    }
    static Lex get_lex ();
};
char Scanner::c;
FILE * Scanner::fp;
const char *
Scanner::TW    [] = { "", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
                      "read", "then", "true", "var", "while", "write",   NULL };
 
const char *
Scanner::TD    [] = { "@", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL };
 
const char *
Scanner::CBG   [] = {"", "continue", "break", "goto", "label", NULL };

Lex Scanner::get_lex () {
    enum    state { H, IDENT, NUMB, COM, ALE, NEQ};
    int     d, j;
    string  buf;
    state   CS = H;
    do {
        gc ();
        switch ( CS ) {
            case H:
                if ( c ==' ' || c == '\n' || c== '\r' || c == '\t' ); 
                    else if ( isalpha (c) ) {
                        buf.push_back (c);
                        CS  = IDENT;
                    }
  
                    else if ( isdigit (c) ) {
                        d   = c - '0';
                        CS  = NUMB;
                    }
                    else if ( c== '{' ) { 
                        CS  = COM;
                    }
                    else if ( c == ':' || c == '<' || c == '>' ) { 
                        buf.push_back (c);
                        CS  = ALE; 
                    }
                    else if (c == '@')
                        return Lex ( LEX_FIN );
                    else if (c == '!') {
                        buf.push_back (c);
                        CS  = NEQ;
                    }
                    else {
                        buf.push_back (c);
                        if ( ( j = look ( buf, TD) ) ){
                            return Lex ( (type_of_lex)( j + (int) LEX_FIN ), j );
                        }
                        else
                            throw c;
                    }
                    break;
            case IDENT:
                if ( isalpha (c) || isdigit (c) ) {
                    buf.push_back (c); 
                }
                else {
                    ungetc ( c, fp );
                    if ( (j = look ( buf, TW) ) ) {
                        return Lex ( (type_of_lex) j, j );
                        }
                    else if( ( j = look(buf, CBG)))
                    {   
                        
                        return Lex ( (type_of_lex)(j+41), 41+j );

                    }
                    else {
                        j   = put ( buf );
                        return Lex ( LEX_ID, j );
                    }
                }
                break;
            case NUMB:
                if ( isdigit (c) ) {
                    d = d * 10 + ( c - '0' );
                }
                else {
                    ungetc ( c, fp );
                    return Lex ( LEX_NUM, d );
                }
                break;
            case COM:
                if ( c == '}' ) {
                    CS  = H;
                }
                else if ( c == '@' || c == '{' )
                    throw c;
                break;
            case ALE:
                if ( c == '=' ) {
                    buf.push_back ( c );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                else {
                    ungetc ( c, fp );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                break;
            case NEQ:
                if ( c == '=' ) {
                    buf.push_back(c);
                    j   = look ( buf, TD );
                    return Lex ( LEX_NEQ, j );
                }
                else
                    throw '!';
                break;
    } //end switch
  } while (true);
}
 
ostream & operator<< ( ostream &s, Lex l ) {
    string t;
    if ( l.t_lex <= LEX_WRITE ){
        t = Scanner::TW[l.t_lex];
        }
    else if ( l.t_lex >= LEX_FIN && l.t_lex <= LEX_GEQ )
        t = Scanner::TD[ l.t_lex - LEX_FIN ];
    else if( l.t_lex == LEX_LABEL)
        t = "LBL";
    else if( l.t_lex == LEX_CONT)
        t = "CONT";
    else if( l.t_lex == LEX_BRK)
        t = "BRK";
    else if( l.t_lex == LEX_GT)
        t = "GOTO"   ;
    else if ( l.t_lex == LEX_NUM )
        t = "NUMB";
    else if ( l.t_lex == LEX_ID )
        t = TID[l.v_lex].get_name ();
    else if ( l.t_lex == POLIZ_LABEL )
        t = "Label";
    else if ( l.t_lex == POLIZ_ADDRESS )
        t = "Addr";
    else if ( l.t_lex == POLIZ_GO )
        t = "!";
    else if ( l.t_lex == POLIZ_FGO ) 
        t = "!F";
    else
        throw l;

       {s << '(' << t << ',' << l.v_lex << ");" << endl;}
    return s;
}
 
//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}
 

class Parser :Scanner {

    static Lex l;
    static stack<string> stk;
    static stack <int> st_int;
    static stack <string> brk_stack;
    static stack <type_of_lex> st_lex;
    static void  dec ( type_of_lex type);
    static void  check_id ();
    static void  check_op ();
    static void  check_not ();
    static void  eq_type ();
    static void  eq_bool ();
    static void  check_id_in_read ();
    class S;
    class Y;
    class D;
    class D1;
    class B;
    class P;
    class E;
    class F;
    class T;
    class X;
public:
    struct temp{int pl0; int pl1;};
    struct gtstruct{string str; int index;};
    static temp tmp;
    static gtstruct gt_temp;
    static stack<int> breaking_stack; 
    static stack <temp> tmp_st;
    static vector <gtstruct> gt_st;
    static stack <temp> cycle;
    static vector<Lex> poliz;
    void analyze();
    Parser(const char* s) : Scanner(s) {}

};
Parser::gtstruct Parser::gt_temp;
vector <Parser::gtstruct> Parser::gt_st;
stack<int> Parser::breaking_stack; 
stack <Parser::temp> Parser::cycle;
stack <string> Parser::brk_stack;
Parser::temp Parser::tmp;
stack <Parser::temp> Parser::tmp_st;
Lex Parser::l;
stack<string> Parser::stk;
stack <int> Parser::st_int;
stack <type_of_lex> Parser::st_lex;
vector<Lex> Parser::poliz;
void Parser::dec ( type_of_lex type ) {
    int i;
    while ( !st_int.empty () ) {
        from_st ( st_int, i );
        if ( TID[i].get_declare () ) 
            throw "twice";
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
        }
    }
}
 
void Parser::check_id () {
    if ( TID[l.get_value()].get_declare() )
        st_lex.push ( TID[l.get_value()].get_type () );
    else 
        throw "not declared";
}
 
void Parser::check_op () {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
 
    from_st ( st_lex, t2 );
    from_st ( st_lex, op );
    from_st ( st_lex, t1 );
 
    if ( op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH )
        r = LEX_INT;
    if ( op == LEX_OR || op == LEX_AND )
        t = LEX_BOOL;
    if ( t1 == t2  &&  t1 == t ) 
        st_lex.push (r);
    else
        throw "wrong types are in operation";
    poliz.push_back (Lex (op) );
}
 
void Parser::check_not () {
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
    else  
        poliz.push_back ( Lex (LEX_NOT) );
}
 
void Parser::eq_type () {
    type_of_lex t;
    from_st ( st_lex, t );
    if ( t != st_lex.top () )
        throw "wrong types are in :=";
    st_lex.pop();
}
 
void Parser::eq_bool () {
    if ( st_lex.top () != LEX_BOOL )
        throw "expression is not boolean";
    st_lex.pop ();
  }
 
void Parser::check_id_in_read () {

    if ( !TID [l.get_value()].get_declare() )
        throw "not declared";
}

class Parser::B{
public:
    void parse();
};
class Parser::E{
public:
    void parse();
};
class Parser::F{
public:
    void parse() 
    {
        //std::cerr<<"ANALYZER F_IN \n";
        if(l.get_type() == LEX_ID)
        {
            check_id();
            poliz.push_back ( Lex ( LEX_ID, l.get_value() ) );
            l = get_lex();
        }
        else if(l.get_type() == LEX_NUM)
        {
            poliz.push_back ( l );
            st_lex.push(LEX_INT);
            l = get_lex();
        }
        else if ( l.get_type() == LEX_TRUE ) {
            st_lex.push ( LEX_BOOL );
            poliz.push_back ( Lex (LEX_TRUE, 1) );
            l = get_lex ();
        }
        else if ( l.get_type() == LEX_FALSE) {
            st_lex.push ( LEX_BOOL );
            poliz.push_back ( Lex (LEX_FALSE, 0) );
            l = get_lex ();
        }
        else if(l.get_type() == LEX_NOT)
        {
            l.get_type();
            stk.push("(FF");
            E().parse();
            if (stk.top() == "(FF") {stk.pop();}
            else throw l;
            check_not();
        }
        else if(l.get_type() == LEX_LPAREN){ 
            //F1
            l = get_lex();
            stk.push("(FE");
            // std::cerr <<  "330 E Begin " << l;
            E().parse();
            // std::cerr <<  "330 E END " << l;

            if (stk.top() == "(FE") {stk.pop();}
            else throw l;
            if(l.get_type() == LEX_RPAREN){ 
                l = get_lex();
            }
            else throw l;
        }
        else throw l;
        //std::cerr<<"ANALYZER F_OUT \n";

    }
};
class Parser::T{
public:
void parse()
{
    //TBEG;
    //std::cerr<<"ANALYZER T_IN \n";

    stk.push("(TF1");
    // std::cerr <<  "372 F Begin " << l;
    F().parse();
    // std::cerr <<  "372 F END" << l;
    if (stk.top() == "(TF1") {stk.pop();}
    else{throw l;}
    
    //T1

    while(l.get_type() == LEX_AND || l.get_type() == LEX_TIMES || l.get_type() == LEX_SLASH)
    {
            st_lex.push (  l.get_type() );
            l = get_lex();
            stk.push("(TF2");
            // std::cerr <<  "387 F Begin " << l;
            F().parse();
            // std::cerr <<  "387 F END" << l;
            if (stk.top() == "(TF2") {stk.pop();}
            else throw l;
            check_op ();
    }
    //std::cerr<<"ANALYZER T_OUT \n";

    }
};
class Parser::X{
public:
    void parse()
    {
    //std::cerr<<"ANALYZER X_IN \n";
    //T
    stk.push("(XT1");
    // std::cerr <<  "398 T Begin " << l;
    T().parse();
    // std::cerr <<  "398 T END " << l;
    if (stk.top() == "(XT1") {stk.pop();}
    else throw l;
    //X1
    while(l.get_type() == LEX_OR || l.get_type() == LEX_PLUS || l.get_type() == LEX_MINUS){
        {
        st_lex.push ( l.get_type() );
        l = get_lex();
        stk.push("(XT2");
        // std::cerr <<  "413 T Begin " << l;
        T().parse();
        // std::cerr <<  "413 T END " << l;
        if (stk.top() == "(XT2") {stk.pop();}
        else throw l;
        check_op ();
            }
        }
    //std::cerr<<"ANALYZER X_OUT \n";

    }
};
void Parser::E::parse(){
{
        //EBEGIN
        //std::cerr<<"ANALYZER E_IN \n";

        stk.push("(EX1");
        // std::cerr <<  "422 X BEGIN " << l;
        X().parse();
        // std::cerr <<  "422 X END " << l;
        if (stk.top() == "(EX1") {stk.pop();}
        else throw l;
        //E1
        if ( l.get_type() == LEX_EQ  || l.get_type() == LEX_LSS || l.get_type() == LEX_GTR ||
         l.get_type() == LEX_LEQ || l.get_type() == LEX_GEQ || l.get_type() == LEX_NEQ ) {
            //E2
            st_lex.push ( l.get_type() );
            l = get_lex();
            stk.push("(EX2");
            // std::cerr <<  "430 X Begin " << l;
            X().parse();
            // std::cerr <<  "430 X END " << l;
            if (stk.top() == "(EX2") {stk.pop();}
            else throw l;
            check_op();
        }

        //std::cerr<<"ANALYZER E_OUT \n";

    
    }
}


//ПРОБЛЕМА В eq_bool eq_
class Parser::S
{
public:
    void parse()
    {
     
        //std::cerr<<"ANALYZER S_IN \n";

        if (l.get_type() == LEX_CONT)
        {   if(brk_stack.empty()){throw l;}
            if (brk_stack.top() == "(CYS") {
                poliz.push_back(Lex(POLIZ_LABEL, cycle.top().pl0));
                poliz.push_back(Lex(POLIZ_GO));
            }
            else { 
                throw l;
            }
            
            l = get_lex();
        }
        else if (l.get_type() == LEX_BRK){
            if(brk_stack.empty()){throw l;}
            if (brk_stack.top() == "(CYS") {
                breaking_stack.push(poliz.size());
                poliz.push_back(Lex()); 
                poliz.push_back(Lex(POLIZ_GO));
            } else {
                throw l;
            }
            l = get_lex();
        }   
        else if(l.get_type() == LEX_GT)
        {
            l = get_lex();
            check_id_in_read();
            gt_temp.str = TID[l.get_value()].get_name();
            gt_temp.index = poliz.size();
            gt_st.push_back(gt_temp);
            l = get_lex();

            poliz.push_back(Lex()); 
            poliz.push_back(Lex(POLIZ_GO));
            if (l.get_type() != LEX_SEMICOLON) throw l;
            else l = get_lex();
            stk.push("(GTS");
            S().parse();
            if (stk.top() == "(GTS") {stk.pop();}
            else throw l; 
        }
        else if (l.get_type() == LEX_ID)
        {
            //S1
            check_id();
            poliz.push_back (Lex ( POLIZ_ADDRESS, l.get_value() ) );
            Lex new_lex = l;
            l = get_lex();
            if (l.get_type() == LEX_ASSIGN)
            {
                //S2
                l = get_lex();
                stk.push("(SE1");
                //E
                // std::cerr <<  "452 E Begin " << l;
                E().parse();
                // std::cerr <<  "452 E END " << l;
                //SEND
               
                if (stk.top() == "(SE1") {stk.pop();}
                else throw l;
                eq_type();
                poliz.push_back(Lex(LEX_ASSIGN));
            }
            else if (l.get_type() == LEX_COLON)
            {
                     for (int i = 0; i < gt_st.size(); i++) {
                        if(gt_st[i].str == TID[new_lex.get_value()].get_name())                        
                        {poliz[gt_st[i].index] = Lex(POLIZ_LABEL,poliz.size());}
                    }
                    l = get_lex();
                    stk.push("(LBS");
                    S().parse();    
                    if(stk.top() == "(LBS"){stk.pop();}
                    else throw;

            }
            else throw l;
            
        }
        else if(l.get_type() == LEX_IF)
        {
            //S3
            
            l = get_lex();
            stk.push("(SE2");
                //E
            // std::cerr <<  "466 E Begin " << l;
            E().parse();
            // std::cerr <<  "466 E END " << l;
                //SEND
            if (stk.top() == "(SE2") {stk.pop();}
            else throw l;
            eq_bool();

            tmp.pl0 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            //S4
            if(l.get_type() == LEX_THEN)
            {
                //S5
                l = get_lex();
                stk.push("(SS1");
                tmp_st.push(tmp);
                S().parse();     
                if (stk.top() == "(SS1") {stk.pop();}
                else throw l;
                from_st(tmp_st, tmp);
                tmp.pl1 = poliz.size();
                poliz.push_back(Lex()); 
                poliz.push_back(Lex(POLIZ_GO));
                poliz[tmp.pl0]= Lex(POLIZ_LABEL, poliz.size());
                //S6
                if(l.get_type() == LEX_ELSE)
                {   
                    //S7
                    l = get_lex();
                    stk.push("(SS2");
                    tmp_st.push(tmp);
                    // std::cerr <<  "485 S Begin " << l;    
                    S().parse();
                    // std::cerr <<  "485 S Begin " << l;
                    //SEND
                    if (stk.top() == "(SS2") {stk.pop();}
                    else throw l; 
                    tmp = tmp_st.top();
                    tmp_st.pop();
                    // tmp = tmp_st.pop(); ?????????????????????????????? 
                    poliz[tmp.pl1]= Lex(POLIZ_LABEL, poliz.size()); 
                }

                else throw l;
            }
            else throw l;
        }
        else if(l.get_type() == LEX_WHILE)
        {
            //S8
            
            l = get_lex();
            stk.push("(SE3");
            brk_stack.push("(CYS");
            tmp.pl0 = poliz.size();
            // std::cerr <<  "500 E Begin " << l;
            E().parse();
            // std::cerr <<  "500 E END " << l;
            if (stk.top() == "(SE3") {stk.pop();}
            else throw l;
            eq_bool();
            tmp.pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            //S9
            if(l.get_type() == LEX_DO)
            {
                //S10
                l = get_lex();
                cycle.push(tmp);
                tmp_st.push(tmp);
                // std::cerr <<  "512 S Begin " << l;
                stk.push("(SS3");

                S().parse();
                // std::cerr <<  "512 S END " << l;
                if (stk.top() == "(SS3") {stk.pop();}
                else throw l;
                from_st(tmp_st, tmp);
                
                poliz.push_back(Lex(POLIZ_LABEL, tmp.pl0));
                poliz.push_back(Lex(POLIZ_GO));
                if(!breaking_stack.empty()){poliz[breaking_stack.top()] = Lex(POLIZ_LABEL, poliz.size());
                    breaking_stack.pop();}
                poliz[tmp.pl1] = Lex(POLIZ_LABEL, poliz.size());
                brk_stack.pop();

            }
            
            else{ throw l;}
            
        }
        else if(l.get_type() == LEX_READ )
        {
            //S11
            l = get_lex();
            if(l.get_type() == LEX_LPAREN){
                //S12
                l = get_lex();
                if(l.get_type() == LEX_ID)
                {     //S3
                    check_id_in_read();
                    poliz.push_back(Lex(POLIZ_ADDRESS, l.get_value()));
                    l = get_lex();
                }
                else throw l;
                if (l.get_type() == LEX_RPAREN)
                {
                //SEND  
                l = get_lex();
                poliz.push_back(Lex(LEX_READ));
                }
                else throw l;
            }
            else throw l;
        }
        else if(l.get_type() == LEX_WRITE)
        {
            
            l = get_lex();
            //S14
            if(l.get_type() == LEX_LPAREN){
                //S15
                l = get_lex();
                stk.push("(SE4");
                // std::cerr <<  "547 E Begin " << l;
                E().parse();
                // std::cerr <<  "547 E END " << l;
                if (stk.top() == "(SE4") {stk.pop();}
                else{throw l;}
                //S16
                st_lex.pop();
                if(l.get_type() == LEX_RPAREN)
                {
                        l = get_lex();
                        poliz.push_back(Lex(LEX_WRITE));
                }
                else{throw l;}
            }
            else{throw l;}
        }
        
        else {
            stk.push("(SB");
            // std::cerr <<  "565 B Begin " << l;
            B().parse();
            // std::cerr <<  "565 B END " << l;    
            if (stk.top() == "(SB") stk.pop();
            else {throw l;}
        }
        //std::cerr<<"ANALYZER S_OUT \n";

    }
};


void Parser::B::parse()
   {    
        //std::cerr<<"ANALYZER B_IN \n";
    
        if(l.get_type() == LEX_BEGIN){
            l = get_lex();

            stk.push("(BS");
                //Sbeg
            // std::cerr <<  "583 S Begin " << l;

            S().parse();
                //Send
            // std::cerr <<  "583 S END " << l;

            if (stk.top() == "(BS") stk.pop();
            else throw l;
            while(l.get_type() == LEX_SEMICOLON)
            {
                l = get_lex();
                stk.push("(BS");
                //Sbeg

                // std::cerr <<  "593 S Begin " << l;

                S().parse();
                //Send
                // std::cerr <<  "593 S END " << l;
                
                if (stk.top() == "(BS") stk.pop();
                else throw l;

            }
                if (l.get_type() == LEX_END)
                {
                    l = get_lex();
                }
        }
        else if (l.get_type() == LEX_END)
            {
               ;
            }
        else throw l;
        //std::cerr<<"ANALYZER B_OUT \n";
}
  



class Parser::D {
    public:
        void parse(){
            //Dbegin
            //std::cerr<<"ANALYZER D_OUT\n";

            if(l.get_type() != LEX_ID)
            {
                    throw l;
            }
            else{
                st_int.push (l.get_value() );
                l = get_lex();

                while( l.get_type() == LEX_COMMA)
                {
                    l = get_lex();
                    if(l.get_type() != LEX_ID) {throw l;}
                    else{st_int.push (l.get_value() );l = get_lex();}
                } 
                if(l.get_type() != LEX_COLON)
                {
                    throw l;
                }
                else
                {
                    l = get_lex();
                    if(l.get_type() == LEX_INT)
                    {
                        dec(LEX_INT);
                        l = get_lex();
                    }
                    else 
                        if(l.get_type() == LEX_BOOL)
                        {
                            dec(LEX_BOOL);
                            l =get_lex();
                        }
                        else throw l;
                }
            }
            //std::cerr<<"ANALYZER D_OUT \n";

        }
};

class Parser::D1 {
    public: 
    void parse();
};

class Parser::Y {
    public:
        void parse(){
            //Ybegin
            //std::cerr<<"ANALYZER Y_OUT \n";
            if (l.get_type() == LEX_VAR)
               {l = get_lex();
                stk.push("(YD");
            //Dstart

                // std::cerr <<  "648 D Begin " << l;

                D().parse();
                //Dend
                // std::cerr <<  "648 D END " << l;

                if(stk.top() == "(YD") stk.pop();
                else throw l;
                while(l.get_type() == LEX_COMMA)
                {
                    l = get_lex();

                    stk.push("(YD");
                    //Dstart

                    // std::cerr <<  "664 D Begin " << l;

                    D().parse();
                    //Dend
                    // std::cerr <<  "664 D END " << l;

                    if(stk.top() == "(YD") stk.pop();
                    else throw l;
                
                }
            }
            else if(l.get_type() == LEX_LABEL)
            {
                l = get_lex();
                stk.push("(LD");
                D1().parse();
                if(stk.top() == "(LD") stk.pop();
                else throw l;
                while(l.get_type() == LEX_COMMA)
                {
                    l = get_lex();
                    stk.push("(LD");
                    D1().parse();
                    if(stk.top() == "(LD") stk.pop();
                    else throw l;
                
                }
            }
            else throw l;
            //Y1
            //std::cerr<<"ANALYZER Y_OUT \n";

        }
};


void Parser::D1::parse(){
            //Dbegin
            //std::cerr<<"ANALYZER D_OUT\n";
            if(l.get_type() != LEX_ID)
            {
                    throw l;
            }
            else{
                st_int.push (l.get_value() );
                l = get_lex();
                while( l.get_type() == LEX_COMMA)
                {
                    l = get_lex();
                    if(l.get_type() != LEX_ID) {throw l;}
                    else{st_int.push (l.get_value() );l = get_lex();}
                } 
                if(l.get_type() == LEX_SEMICOLON)
                {
                    dec(LEX_LABEL);
                    l = get_lex();
                    stk.push("(D1Y");
                    Y().parse();

                    if(stk.top() == "(D1Y") stk.pop();
                }
            }
            //std::cerr<<"ANALYZER D_OUT \n";

}


class Parser::P {
public:
    void parse() {
        //Pbeg:
        //std::cerr<<"ANALYZER P_IN \n";

        if (l.get_type() == LEX_PROGRAM)
            l = get_lex();
        else {throw l;}
        //P1:

        // std::cerr <<  "666 Y Begin " << l;
        stk.push("(PY");
        //Ybeg:

        Y().parse();
        //Yend:

        // std::cerr <<  "666 Y End " << l;
        if (stk.top() == "(PY") stk.pop();
        else throw l;
        //P2:
        if (l.get_type() == LEX_SEMICOLON)
            l = get_lex();
        else throw l;
        //P3:
        stk.push("(PB");
        //Bbeg:
        // std::cerr <<  "683 B Begin " << l;
        
        B().parse();
        //Bend:

        // std::cerr <<  "683 B END" << l;
        

        if (stk.top() == "(PB") {stk.pop();}
        else throw l;
        //P4:
        if (l.get_type() != LEX_FIN) throw l;
        //std::cerr<<"ANALYZER P_OUT \n";

    }
};

 
class Interpretator : Parser {
public:
    struct temp{int pl0; int pl1;};
    static temp tmp;
    static stack <temp> tmp_st;
    static vector<Lex> poliz;
    Interpretator(const char* s)   : Parser(s)  {}
    void interpretation(){
        Parser::analyze();
        static vector<Lex> poliz = Parser::poliz;
        Lex pc_el;
        stack < int > args;
        int i, j, index = 0, size = poliz.size();
        while ( index < size ) {
            pc_el = poliz [ index ];
            switch ( pc_el.get_type () ) {
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
                args.push ( pc_el.get_value () );
                break;
            case LEX_ID:
                i = pc_el.get_value ();
                if ( TID[i].get_assign () ) {
                  args.push ( TID[i].get_value () );
                  break;
                }
                else
                  throw "POLIZ: indefinite identifier";
            case LEX_NOT:
                from_st ( args, i );
                args.push( !i );
                break;
            case LEX_OR:
                from_st ( args, i ); 
                from_st ( args, j );
                args.push ( j || i );
                break;
            case LEX_AND:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j && i );
                break;
            case POLIZ_GO:
                from_st ( args, i );
                index = i - 1;
                break;
            case POLIZ_FGO:
                

                from_st ( args, i );
                from_st ( args, j );
              
                if ( !j ) {index = i - 1 ;}
                break;
            case LEX_WRITE:
                from_st ( args, j );
                cout << j << endl;
                break;
            case LEX_READ:
                int k;
                from_st ( args, i );
                if ( TID[i].get_type () == LEX_INT ) {
                    cout << "Input int value for" << TID[i].get_name () << endl;
                    cin >> k;
                }
                else {
                    string j;
                    while (1) {
                        cout << "Input boolean value (true or false) for" << TID[i].get_name() << endl;
                        cin >> j;
                        if ( j != "true" && j != "false" ) {
                            cout << "Error in input:true/false" << endl;
                            continue;
                        }
                        k = ( j == "true" ) ? 1 : 0;
                        break;
                    }
                }
                TID[i].put_value (k);
                TID[i].put_assign ();
                break;
            case LEX_PLUS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i + j );
                break;
            case LEX_TIMES:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i * j );
                break;
            case LEX_MINUS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j - i );
                break;
            case LEX_SLASH:
                from_st ( args, i );
                from_st ( args, j );
                if (i) {
                    args.push ( j / i );
                    break;
                }
                else throw "POLIZ:divide by zero";
            case LEX_EQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i == j );
                break;
            case LEX_LSS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j < i );
                break;
            case LEX_GTR:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j > i );
                break;
            case LEX_LEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j <= i );
                break;
            case LEX_GEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j >= i );
                break;
            case LEX_NEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j != i );
                break;
            case LEX_ASSIGN:
                from_st ( args, i );
                from_st ( args, j );
                TID[j].put_value (i);
                TID[j].put_assign (); 
                break;
            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;

            
                
        }

    }
};


void Parser::analyze() {
    std::cerr<<"ANALYZER START \n";
    l = get_lex();
    stk.push("(start");
    P().parse();
    if (stk.top() == "(start") stk.pop();
    else throw l;
    std::cerr<<"SUCCESSFUL! \n";
    std::cerr<<"---------------------------------------";
    std::cerr<<"\n";
    std::cerr<<"OUTPUT:\n\n";
    
    // std::cerr<<"POLIZ! \n";
    // for (auto it = poliz.begin(); it != poliz.end(); ++it) {
    //     std::cerr<<*it;

    //  }

}
int main (int argc, char *argv[]) {
    if (argc == 2){
        {
            std::cerr<<"TEST :  1 " << " | FILE : " << argv[1] << std::endl;
            try{Interpretator I(argv[1]);I.interpretation ();
        ;}
            catch(Lex l){
                std::cout<< "wrong lexeme\t"<< l <<std::endl;
            }
            catch(char c){
                std::cout<< "wrong symbol\t"<< c << std::endl;
            }
            catch(const char* c){
                    std::cerr<<"---------------------------------------";
    std::cerr<<"\n";
                std::cout<<"ERROR!\n\n"<<std::endl;
                std::cout<<"err_info : ";
                std::cout<<c<<std::endl;
            }
            std::cerr<<'\n';   
        }
    }
    else cerr<<"NO FILE FOUND\n";
    
    
    // Lex l;
    // Scanner S("test5.txt");
    // l=S.get_lex();
    // while(l.get_type()!=LEX_FIN)
    // {
    //     cout<<l<<endl;
    //     l=S.get_lex();
    // };
}