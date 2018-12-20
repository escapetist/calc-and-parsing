#include <iostream>
#include <windows.h>
#include <cmath>
#include <string>

using namespace std;

void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    // Get the number of cells in the current buffer
    if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
    cellCount = csbi.dwSize.X *csbi.dwSize.Y;

    // Fill the entire buffer with spaces
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR) ' ',
        cellCount,
        homeCoords,
            &count
        )) return;

    // Fill the entire buffer with the current colors and attributes
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
        )) return;

    // Move the cursor home
    SetConsoleCursorPosition( hStdOut, homeCoords );
}

/*
    see Bjarne Stroustup's book for deeper details, I implemented a similar approach
    program:
	END			   // END is end-of-input
	expr_list END

This is the basic grammar:
    expr_list:
	expression PRINT	   // PRINT is semicolon
	expression PRINT expr_list

    expression:
	expression + term
	expression - term
	term

    term:
	term / secondary
	term * secondary
	secondary

	secondary:
	primary
	secondary^primary

    primary:
	NUMBER
	NAME
	( expression )
	This is in fmla mode: NAME
                            NAME = expression
                                - primary
*/

class console //handles console
{
        private:
            int no_of_errors;
            enum Token_value        //will come in handy later
            {
                NAME, NUMBER,	END = 'q', PI ='p',
                PLUS='+',	MINUS='-',	MUL='*',	DIV='/',
                PRINT=';',	LP='(',		RP=')', POW ='^'
            };

            Token_value curr_tok ;
            double number_value;
            char* string_value;

        public:
            console():no_of_errors(0),curr_tok(PRINT){ string_value =new char[5]; }; //initializing values
            double error(const char* s)
            {
                no_of_errors++;
                cerr << "error: " << s << '\n';
                return 1;
            }
            int open()
            {
                while (cin)
                {
                    no_of_errors = 0; //for no unexpected artifact garbage values
                    cout<<">";
                    get_token();
                    if (curr_tok == END) break;
                    if (curr_tok == PRINT) continue;
                    if (!no_of_errors)
                        cout << exprn(false) << '\n';
                }
                return no_of_errors;
            }
            Token_value get_token()
            {
                char ch;

                do
                {
                    if(!cin.get(ch))    // skip whitespace
                        return curr_tok = END;
                } while (ch!='\n' && isspace(ch));

                switch (ch) {
                case 'q':
                    return curr_tok=END;
                case ';':
                case '\n':
                    return curr_tok=PRINT;
                case '*':
                case '/':
                case '+':
                case '^':
                case '-':
                case '(':
                case ')':
                case '=':
                    return curr_tok=Token_value(ch);
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case '.':
                    cin.putback(ch); //makes previous char from istream extractable
                    cin >> number_value;
                    return curr_tok=NUMBER;
                case 'p':
                    return curr_tok = PI;
                default:			//  word or error
                    if(isalpha(ch)&&(ch!='q'))
                        {
                            cin.putback(ch);
                            cin.get(string_value, 5);
                            return curr_tok = NAME;
                        }
                    error("bad token");
                    return curr_tok=PRINT;
                }
            }

            double exprn(bool);	// cannot do without it

            double prim(bool get)		// handle primaries as explained in the comments above this class
            {
                if (get) get_token();

                switch (curr_tok)
                {
                    case NUMBER:		// floating-point constant
                    {
                        double v = number_value;
                        get_token();
                        return v;
                    }
                    case MINUS:		// unary minus
                        return -prim(true);
                    case LP:
                    {
                        double e = exprn(true);
                        if (curr_tok != RP) return error(") expected");
                        get_token();		// eat ')'
                        return e;
                    }
                    case NAME:
                        {
                            double e;
                            if(!strcmp(string_value,"sin("))
                                e = sin(exprn(true));
                            else if(!strcmp(string_value,"cos("))
                                e = cos(exprn(true));
                            else if(!strcmp(string_value,"tan("))
                                e = tan(exprn(true));
                            else if(!strcmp(string_value,"asn("))
                                e = asin(exprn(true));
                            else if(!strcmp(string_value,"acs("))
                                e = acos(exprn(true));
                            else if(!strcmp(string_value,"atn("))
                                e = atan(exprn(true));
                            else if(!strcmp(string_value,"exp("))
                                e = exp(exprn(true));
                            else if(!strcmp(string_value,"lnt("))
                                e = log(exprn(true));
                            else if(!strcmp(string_value,"log("))
                                e = log10(exprn(true));
                            if (curr_tok != RP) return error(") expected");
                            get_token();		// eat ')'
                            return e;
                        }
                    case PI:
                    {
                        double v= 3.14159265;
                        get_token();
                        return v;
                    }
                    default:
                        return error("primary expected");
                }
            }
            double sec(bool get) //handles secondaries
            {
                double left = prim(get);
                for (;;)
                    switch(curr_tok)
                    {
                        case POW:
                            left = pow(left,prim(true));
                            break;
                        default:
                            return left;
                    }
            }

            double term(bool get)		// multiply and divide
            {
                double left = sec(get);

                for (;;)
                    switch (curr_tok)
                    {
                        case MUL:
                            left *= sec(true);
                            break;
                        case DIV:
                            if (double d = sec(true))
                            {
                                left /= d;
                                break;
                            }
                            return error("divide by 0");

                        default:
                            return left;
                    }
            }
};
double console::exprn(bool get)		// add and subtract
{
    double left = term(get);

    for (;;)				// infinite loop
        switch (curr_tok)
        {
            case PLUS:
                left += term(true);
                break;
            case MINUS:
                left -= term(true);
                break;
            default:
                return left;
        }
}


class fmla:public console //as fmla screen is also a console screen with few modifications
{

};
class wallOfText;
class enggmode //as engg mode also has options to choose from and is basically a wall of text
{
    public:
        void menu()
        {
            cout<<"Under Development. Come back later, thanks!"<<endl;
            char a;
            cin>>a;
            if(cin)
                return;
        }

};

class wallOfText //stores all big lines of text and instructions, including any menus
{
    private:
        console a;
        enggmode b;
    public:
        void virtual menu() //for the main screen
        {
            ClearScreen();
            cout << "-----------------Welcome to the All-in-One Engineering calculator-----------------" << endl;
            cout << "a. Console"<<endl;
            cout << "b. Engg Mode"<<endl;
            cout << "c. Fmla Mode"<<endl;
            cout << "d. How to use the calculator"<<endl;
            cout << "q. Exit"<<endl;
            cout << "------------------------Developed by: Pratik, Rajil, Stuti------------------------" <<endl;
            char select; //to select menu option
            cin>>select;
            switch(select)
            {
                case 'a':
                    ClearScreen();
                    a.open();
                    menu();
                case 'b':
                    ClearScreen();
                    b.menu();
                    menu();
                case 'q':
                    exit(0);
                case 'd':
                    ClearScreen();
                    instructions();
            }
        }
        void instructions()
        {
            cout<<"Greetings! Welcome to the All-in-One Engineering Calculator, here are the basics of the menus"<<endl;
            cout<<"The console opens up a blank screen where arithmetic calculations can be done "<<endl;
            cout<<"In the Engg mode, you have access to more options such as matrices, differentials, equations"<<endl;
            cout<<"In the fmla mode, simply type the formula and we'll solve for the unknown of your choice in the formula!"<<endl;
            cout<<"Press 'q' to go back on any menu! Pressing q on main menu exits the program!"<<endl;
            cout<<"Keywords: sin/cos/tan - sin(angle in radian)/cos(angle in radian)/tan(angle in radian)"<<endl;
            cout<<"          inverse trigonometry - arcsin/arccos/arctan - asn(value)/acs(value)/atn(value)"<<endl;
            cout<<"          exponential power - exp(expression)"<<endl;
            cout<<"          natural log - lnt(expression)"<<endl;
            cout<<"          natural log base 10 - log(expression)"<<endl;
            cout<<"          a raised to the power b a^b"<<endl;
            cout<<"          pi =p "<<endl;
            char a;
            cin>>a;
            if(a == 'q')
            {
                ClearScreen();
                menu();
            }
        }
};

int main()
{
    wallOfText main;
    main.menu();
    return 0;
}
