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

//The classes below handle matrices and equations
class matrix;
class equation;
class matrix
{
private:
    double elements[25]; //matrices with upto 25 elements supported
    double cofactors[9]; //cofactor matrix for upto an order of 3
    double transpose[25]; //to store elements of the transpose of the matrix
    double adjoint[9]; //to store the elements of the adjoint of the matrix
    double inverse[9]; //to store the elements of the inverse of the matrix
    double det; //determinant ko laagi
    bool null; //to see if the matrix is empty;
public:
    int rows; //rows cnt
    int columns; //columns cnt
    int mta(int m, int n) //matrix-to-array: locates the matrix element inside the array
    {
        return (m * columns + n);
    }
    matrix(bool input = false) //input is not taken by default
    {
            do
            {
                cout<<"Rows: ";
                do
                {
                    cin>>rows;
                    if(!(cin))
                        cout<<"please enter a number"<<endl;
                }while(!cin);
                cout<<"Columns: ";
                do
                {
                    cin>>columns;
                    if(!cin)
                        cout<<"please enter a number"<<endl;
                }while(!cin);
                if(rows+columns>16)
                    cout<<"Error, we only support a maximum of 4*4 matrices"<<endl;
            }while((rows+columns>16));
            init(rows, columns);

    }
   /* matrix(int m, int n, bool input) //if the matrix is not to be initialized from the keyboard
    {
        if(input == 0)
        {
            rows = m;
            columns = n;
            null = true;
        }
        else
        {
            init(m, n);
        }

    }
    matrix(int m)
    {
        init(m, m);
    }
    matrix(int m, int n) //constructs a m X n matrix and stores the elements in a 1D array
    {
        init(m, n);
    }*/
    matrix(int m, int n, double arr[]) //when an array is to be arranged into a matrix; useful for equation solving
    {
        rows = m;
        columns = n;
        for(int i = 0; i < m; i++)
        {
            for(int j = 0; j < n; j++)
            {
                elements[mta(i, j)] = arr[mta(i, j)];
            }
        }
        null = false;
    }

    class nullMatrix{}; //if the matrix is empty
    class noDeterminant{}; //exception class in case the determinant does not exist
    class noInverse{}; //exception class in case the inverse does not exist
    class noCofactor{}; //exception class in case the cofactor does not exist, happens for non-square matrices;
    class noAdjoint{}; //exception class in case the adjoint doesn not exist, happens for non-square matrices;
    class outofBounds{}; //if the product matrix has more than 25 elements
    class noProduct{}; //if the operand matrices are non-conformable for multiplication

    void init(int m, int n); //initialilzes the matrix

    double determ(int num); //this function is for calculating the determinant of 3 X 3 matrices; also useful for 4 X 4

    double determinant(); //to check the order and obtain the corresponding determinant with the help of determ

    void cof(); //function to calculate the cofactor matrix;

    void transposer(); //function to obtain the transpose of the matrix

    void adjointer(); //function to evaluate and store the adjoint of a matrix

    void invert(); //function to calculate and store the inverse of a matrix

    matrix operator * (matrix);

    void show_matrix()
    {
        if(null == true)
            throw nullMatrix();
        cout<<"The given matrix is: "<<endl;
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                cout<<elements[mta(i, j)]<<"\t";
            }
            cout<<endl;
        }
    }
    void show_transpose()
    {
        transposer();
        cout<<"The transpose of the given matrix is:"<<endl;
        for(int i = 0; i < columns; i++)
        {
            for(int j = 0; j < rows; j++)
            {
                cout<<transpose[i * columns + j]<<"\t";
            }
            cout<<endl;
        }
    }
    void show_cofactors()
    {
        cof();
        cout<<"The cofactor matrix of the given matrix is: "<<endl;
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                cout<<cofactors[mta(i, j)]<<"\t";
            }
            cout<<endl;
        }
    }
    void show_adjoint()
    {
        adjointer();
        cout<<"The adjoint of the given matrix is: "<<endl;
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                cout<<adjoint[mta(i, j)]<<"\t";
            }
            cout<<endl;
        }
    }
    void  show_inverse()
    {
        invert();
        cout<<"The inverse of the given matrix is: "<<endl;
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                cout<<inverse[mta(i, j)]<<"\t";
            }
            cout<<endl;
        }
    }
};

void matrix::init(int m, int n)
{
        rows = m;
        columns = n;
        for(int i = 0; i < rows; i++) //row iteration
        {
            for(int j = 0; j < columns; j++) //column iteration
            {
                cout<<"a("<<i + 1<<","<<j + 1<<") = "; cin>>elements[mta(i, j)];
            }
        }
        null = false;
}

double matrix::determinant()
{
    if(null == true)
        throw nullMatrix();
    if(rows!=columns)
    {
        throw noDeterminant();
    }
    if(rows == 1)
    {
        det = elements[0];
        return det;
    }
    else if(rows == 2)
    {
        det = elements[0] * elements[3] - elements[1] * elements[2]; //ad-bc
        return det;
    }
    else if(rows == 3)
    {
        det = determ(0);
        return det;
    }
    else if(rows == 4)
    {
        double a, b, c, d;
        a = elements[0] * determ(1);
        b = elements[1] * determ(2);
        c = elements[2] * determ(3);
        d = elements[3] * determ(4);
        det = a - b + c - d;
        return det;
    }
    else
    {
        cout<<"Uunhandled exception: determinant of upto 4 X 4 (square) matrices only\n";
    }
}

double matrix::determ(int num)
{
    double submat[9]; //submatrix of order 3
    double tmp = 0;
    int c_pos; //column position
    int r_pos; //row position
    r_pos = num == 0? 0: 1;
    for(int i = 0; i < 3; i ++)
    {
        for(int j = 0; j < 3; j++)
        {
            c_pos = (num + j) % 4;
            submat[i * 3 + j] = elements[mta(i+r_pos, c_pos)];
        }
    }

    //after the 3 X 3 matrix has been extracted to submat, calculate the determinant
    int  c, d; //need this below
    for(int j = 0; j < 3; j++)
    {
            c = (j + 1) % 3;
            d = (j + 2) % 3;
            tmp += submat[j] *(submat[1 * 3 + c] * submat[2 * 3 + d]- submat[1 * 3 + d] * submat[2 * 3 + c]);
    }
    return tmp;
}

void matrix::cof()
{
    if(null == true)
        throw nullMatrix();
    if(rows != columns)
        throw noCofactor();
   if(rows == 1)
   {
       cofactors[0] = 1;
   }
   else if(rows == 2)
   {
       for(int i = 0; i < 4; i++)
       {
           cofactors[i] = pow(-1, i) * elements[3 - i];
       }
   }
   else if(rows == 3)
   {
       int x1, x2, y1, y2; //required for clarity as in determinants
       for(int i = 0; i < 3; i++)
       {
           for(int j = 0 ; j < 3; j++)
           {
               x1 = (i + 1) % 3;
               x2 = (i + 2) % 3;
               y1 = (j + 1) % 3;
               y2 = (j + 2) % 3;
               cofactors[mta(i, j)] = elements[mta(x1, y1)] * elements[mta(x2, y2)]
                                        - elements[mta(x2, y1)] * elements[mta(x1, y2)];
           }
       }
   }
   else
   {
       cout<<"Unhandled Exception: Cofactors upto 3 X 3 matrices only"<<endl;
   }
}

void matrix::transposer()
{
    if(null == true)
        throw nullMatrix();
    for(int i = 0; i < columns; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            transpose[mta(i, j)] = elements[mta(j, i)];
        }
    }
}

void matrix::adjointer()
{
    if(null == true)
        throw nullMatrix();
    if(rows != columns)
        throw noAdjoint();
    cof();
    for(int i = 0; i < columns; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            adjoint[mta(i, j)] = cofactors[mta(j, i)];
        }
    }
}

void matrix::invert()
{
    if(null == true)
        throw nullMatrix();
    if(rows != columns || det == 0)
        throw noInverse();
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < columns; j++)
        {
            inverse[mta(i, j)] = adjoint[mta(i, j)] / det ;
        }
    }
}

matrix matrix::operator * (matrix m1)
{
    if(rows * m1.columns > 25)
        throw outofBounds();
    if(columns != m1.rows)
        throw noProduct();
    matrix tmp(rows, m1.columns, false);
    for(int i = 0; i < rows; i++)
    {
        for(int k = 0; k < m1.columns; k++)
        {
            for(int j = 0; j < columns; j++)
            {
                tmp.elements[tmp.mta(i, k)] += elements[mta(i, j)] * m1.elements[m1.mta(j, k)];
            }
            cout<<"\t";
        }
        cout<<endl;

    }
    tmp.null = false;
    return tmp;
}

class equation
{
private:
    int noOfvar; //no. of unknown variables
    int x, y, z, w; //upto 4 unknown variablesz
    double coeff[16]; //coefficient matrix array
    double d; //determinant of the coefficient matrix
    double constant[4]; //constant matrix array
    double d1[16]; //for x
    double d2[16]; //for y
    double d3[16]; //for z
    double d4[16]; //for w
public:
    equation()
    {
        init();
    }
    class noOfvarError{}; //exception of either too many unknowns or too less
    class noSolution{}; //exception of no solution

    void takevalues(); //to fill up the coeffient matrix

    void solve();

    void init()
    {
        cout<<"Enter the no. of unknown variables"; cin>>noOfvar;
        if(noOfvar > 4 || noOfvar < 2)
            throw noOfvarError();
        if(noOfvar == 2)
        {
            cout<<"FORMAT: ax + by = c"<<endl;
            takevalues();
            matrix coeffmat(2, 2, coeff);
            d = coeffmat.determinant();
            if(d == 0)
                throw noSolution();
            solve();
        }
        else if(noOfvar == 3)
        {
            cout<<"FORMAT: ax + by + cz = d"<<endl;
            takevalues();
            matrix coeffmat(3, 3, coeff);
            d = coeffmat.determinant();
            if(d == 0)
                throw noSolution();
            solve();
        }
        else
        {
            cout<<"FORMAT: ax + by + cz + dw = e"<<endl;
            takevalues();
            matrix coeffmat(4, 4, coeff);
            d = coeffmat.determinant();
            if(d == 0)
                throw noSolution();
            solve();
        }
    }
    void showsolution()
    {
        cout<<endl<<"The solution:::"<<endl;
        cout<<"x = "<< x<<endl;
        cout<<"y = "<< y<<endl;
        if(noOfvar > 2)
            cout<<"z = "<< z <<endl;
        if(noOfvar > 3)
            cout<<"w = "<< w << endl;
    }
};

void equation::takevalues()
{
    cout<<"Enter the elements of the coefficient matrix:"<<endl;
    for(int i = 0; i < noOfvar; i++)
    {
        for(int j = 0; j < noOfvar; j++)
        {
            cout<<"("<<i + 1 <<","<<j + 1<<") = "; cin>>coeff[i * noOfvar + j];
        }
    }
    cout<<"Enter the elements of the constant matrix:"<<endl;
    for(int i = 0; i < noOfvar; i++)
    {
        cout<<"constant["<<i+1<<"] = "; cin>>constant[i];
    }
}

void equation::solve()
{
   for(int i = 0; i < noOfvar; i++)
   {
       for(int j = 0; j < noOfvar; j++)
       {
           if(j == 0)
           {
               d1[i * noOfvar + j] = constant[i];
           }
           else
           {
               d1[i * noOfvar + j] = coeff[i * noOfvar + j];
           }
           if(j == 1)
           {
               d2[i * noOfvar + j] = constant[i];
           }
           else
           {
               d2[i * noOfvar + j] = coeff[i * noOfvar + j];
           }
           if(j == 2)
           {
               d3[i * noOfvar + j] = constant[i];
           }
           else if(j!=2 && noOfvar > 2)
           {
               d3[i * noOfvar + j] = coeff[i * noOfvar + j];
           }
           if(j == 3)
           {
               d4[i * noOfvar + j] = constant[i];
           }
           else if(j != 3 && noOfvar > 3)
           {
               d4[i * noOfvar + j] = coeff[i * noOfvar + j];
           }
       }
   }
    matrix mx(noOfvar, noOfvar, d1);
    matrix my(noOfvar, noOfvar, d2);
    x = mx.determinant() / d;
    y = my.determinant() / d;

   if(noOfvar > 2)
   {
       matrix mz(noOfvar, noOfvar, d3);
       z = mz.determinant() / d;
   }
   if(noOfvar > 3)
   {
       matrix mw(noOfvar, noOfvar, d4);
       w = mw.determinant() / d;

   }
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
                PRINT=';',	LP='(',		RP=')', POW ='^', PAR ='|'
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
                case '|':
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
                            if (curr_tok != RP) return error("bracket expected");
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
            double sec(bool get) //handles secondaries (power)
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
            double tert(bool get) // handles parallel
            {
                double left = sec(get);
                for(;;)
                    switch(curr_tok)
                    {
                        case PAR:
                            double x;
                            x=sec(true);
                            if(left+x)
                            {
                                left = (left*x)/(left +x);
                                break;
                            }
                            return error("divide by 0");
                        default:
                            return left;
                    }
            }
            double term(bool get)		// multiply and divide
            {
                double left = tert(get);

                for (;;)
                    switch (curr_tok)
                    {
                        case MUL:
                            left *= tert(true);
                            break;
                        case DIV:
                            if (double d = tert(true))
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
double console::exprn(bool get)		// add and subtract the 'get' denotes if it is the first token typed or not
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


class mtx_menu
{
    private:
        char a;
    public:
        void menu()
        {

            ClearScreen();
            cout<<"Input matrix"<<endl;
            try
            {
                    matrix m(true); //true denotes that input is taken when the object is instantiated
                    m.show_matrix();
                    if(m.rows==m.columns)
                        cout<<endl<<"Determinant= "<<m.determinant()<<endl;
                    m.show_transpose();
                    cout<<endl;
                    m.show_cofactors();
                    cout<<endl;
                    m.show_adjoint();
                    cout<<endl;
                    m.show_inverse();
                    cout<<endl;
                    matrix n(true);
                    n.show_matrix();
                    matrix product;
                    product = m * n;
                    product.show_matrix();
            }
                catch(matrix::noDeterminant)
                {
                    cout<<"Determinant does not exist for non-square matrices"<<endl;
                }
                catch(matrix::noAdjoint)
                {
                    cout<<"Adjoint does not exist"<<endl;
                }
                catch(matrix::noCofactor)
                {
                    cout<<"Cofactors do not exist"<<endl;
                }
                catch(matrix::noInverse)
                {
                    cout<<"Inverse does not exist"<<endl;
                }
                catch(matrix::noProduct)
                {
                    cout<<"Given matrices non-conformable for multiplication"<<endl;
                }
                catch(matrix::outofBounds)
                {
                    cout<<"The product matrix has too many elements"<<endl;
                }
                catch(matrix::nullMatrix)
                {
                    cout<<"The matrix is null; nothing to display"<<endl;
                }
                while(1)
                {
                    cin>>a;
                    if(a=='q')
                        return;
                }
        }
};
class eqn_menu
{
    private:
        char a;
    public:
        void menu()
        {
            ClearScreen();
            try
            {
                equation eqn;
                eqn.showsolution();
            }
            catch(equation::noOfvarError)
            {
                cout<<"Incorrect number of variables"<<endl;
            }
            catch(equation::noSolution)
            {
                cout<<"No (unique) solution exists"<<endl;
            }
            while(1)
            {
                cin>>a;
                if(a=='q')
                    return;
            }
        }
};

class enggmode//as engg mode also has options to choose from and is basically a wall of text
{
    private:
        mtx_menu m;
        eqn_menu e;
    public:
        void menu()
        {
            ClearScreen();
            cout<<"Choose: the red pill, or the blue pill?"<<endl;
            cout<<"a. Matrix mode"<<endl;
            cout<<"b. Equation mode"<<endl;
            char a;
            cin>>a;
            switch(a)
            {
                case 'a':
                    m.menu();
                    menu();
                case 'b':
                    e.menu();
                    menu();
                case 'q':
                    ClearScreen();
                    return;

            }

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
            cout << "c. How to use the calculator"<<endl;
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
                case 'c':
                    ClearScreen();
                    instructions();
            }
        }
        void instructions()
        {
            cout<<">Greetings! Welcome to the All-in-One Engineering Calculator, here are the basics of the menus"<<endl;
            cout<<">The console opens up a blank screen where arithmetic calculations can be done "<<endl;
            cout<<">In the Engg mode, you have access to more options such as matrices, differentials, equations"<<endl;
            cout<<">In the fmla mode, simply type the formula and we'll solve for the unknown of your choice in the formula!"<<endl;
            cout<<">Press 'q' to go back on any menu! Pressing q on main menu exits the program!"<<endl;
            cout<<">When in the console, press qa to clear screen!"<<endl;
            cout<<">Keywords:\n\tsin/cos/tan - sin(angle in radian)/cos(angle in radian)/tan(angle in radian)"<<endl;
            cout<<"          inverse trigonometry - arcsin/arccos/arctan - asn(value)/acs(value)/atn(value)"<<endl;
            cout<<"          exponential power - exp(expression)"<<endl;
            cout<<"          natural log - lnt(expression)"<<endl;
            cout<<"          natural log base 10 - log(expression)"<<endl;
            cout<<"          a raised to the power b a^b"<<endl;
            cout<<"          pi =p "<<endl;
            cout<<">Also keep in mind that you MUST use the * operator to denote multiplication"<<endl;
            cout<<">A bracket will not suffice. A bracket will only give outputs of individual brackets"<<endl;
            cout<<">eg (2*5)(9+8) will give outputs 10 and 17, not 170!"<<endl;
            char a;
            cin>>a;
            while(a!='q')
            {
                    cin>>a;
                    if(a == 'q')
                    {
                        ClearScreen();
                        menu();
                    }
            }

        }
};

int main()
{
    wallOfText main;
    main.menu();
    return 0;
}
