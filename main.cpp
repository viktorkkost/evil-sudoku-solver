#include <iostream>
#include <cstdlib>


#define RES_END 0
#define RES_BADNUM 1


using namespace std;

//global vars cuz i don't give a damn
//9x9 board. initially 0s, should contain 1~9 when done
unsigned char sudoku[9][9];
//every index corresponds to a square. every bit corresponds to a number
//and if set, the number is free to use. 1 is (1<<0), 2 is (1<<1) etc.
//all bits start set because all numbers are valid in an empty sudoku
unsigned short rows[9], cols[9], squares[9];
//unimportant
unsigned char temp;

void init()
{
    int i, j;
    for(i = 0; i < 9; ++i)
    {
        for(j = 0; j < 9; ++j)
        {
            sudoku[i][j] = 0;
        }
        rows[i] = 0xFFFF;
        cols[i] = 0xFFFF;
        squares[i] = 0xFFFF;
    }
}

//not actual depth but easier for arithmetics
unsigned char solveSquare(unsigned char depth)
{
    //square 82 means the end of the sudoku is reached successfully. this is
    //before incrementing so depth 80 is actually square 82
    if(depth == 80)
        return RES_END;

    //increment depth. now depth 0 would mean we are on square 1
    depth++;

    //square already entered by user?
    if(sudoku[depth/9][depth%9])
    {
        //if yes, simply go to the next one
        return solveSquare(depth);
    }
    //else, solve that square
    unsigned short availableNums = 0;
    unsigned char numAvailableNums = 0, chosenIndex, res;

    //check which digits are available and add them to the array. also gets the
    //number of available digits so we don't have to iterate over invalid ones
    for(temp = 0; temp < 9; ++temp)
    {
        //if digit is available in this row, col and square...
        if((rows[depth/9] & 1 << temp) &&
        (cols[depth%9] & 1 << temp) &&
        (squares[(depth/27)*3 + (depth%9)/3] & 1 << temp))
        {
            //mark it as available
            availableNums |= 1 << temp;
            //and increment availability counter
            numAvailableNums++;
        }
    }

    //this loop iterates only as many times as there are available digits for the
    //square we're on
    for(; numAvailableNums > 0; --numAvailableNums)
    {
        //choose random number from available ones (numAvailableNums gets
        //decremented in the outer loop). res is the pseudo-index of sorts, if we
        //iterate only over available digits
        res = rand() % numAvailableNums;

        //we store the result in chosenIndex. iterate over it until we see at least
        //res available digits
        chosenIndex = 0;
        temp = 0;
        while(1)
        {
            if(availableNums & 1 << chosenIndex)
                temp++;

            if(temp > res)
                break;

            ++chosenIndex;
        }

        //at this point we've chosen a random index that points to an available digit.
        //mark it as used in rows, cols and squares
        rows[depth/9] &= ~(1 << chosenIndex);
        cols[depth%9] &= ~(1 << chosenIndex);
        squares[(depth/27)*3 + (depth%9)/3] &= ~(1 << chosenIndex);

        //go to next square
        res = solveSquare(depth);

        //returned because...
        if(res == RES_BADNUM)
        {
            //...of a bad number. mark number as free in rows,
            //cols and squares
            rows[depth/9] |= 1 << chosenIndex;
            cols[depth%9] |= 1 << chosenIndex;
            squares[(depth/27)*3 + (depth%9)/3] |= 1 << chosenIndex;

            //set that num to unavailable
            availableNums &= ~(1 << chosenIndex);

            //and go to next iteration
        }
        else
        {
            //end is reached. populate board going backwards
            sudoku[depth/9][depth%9] = chosenIndex+1;
            return res;
        }
    }

    //exhausted all available digits for this square or there were none. so we
    //move up a level
    return RES_BADNUM;
}

void print()
{
    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            if(!(j%3))
                cout << ' ';
            if(sudoku[i][j] == 0)
                cout << '*';
            else
                cout << (unsigned short) sudoku[i][j];
        }
        if(!((i+1)%3))
            cout << '\n';
        cout << '\n';
    }
}

int main() {
    //init global vars
    init();

    //seed RNG
    srand(0);

    char* inp = new char[82];

    if(inp == NULL)
        return 1;

    //read 81 characters
    cout << "Enter puzzle. Left to right and up to down, 0s for empty squares:\n";
    cin.get(inp, 82);

    //iterate over input
    for(int i = 0; i < 81 && inp[i]; ++i)
    {
        //assume only digits were passed. insert non-zeroes
        if(inp[i] - '0')
        {
            sudoku[i/9][i%9] = (unsigned char) (inp[i] - '0');
            
            //digit hasn't been used in this row, col and square?
            if(!(rows[i/9] & 1 << inp[i] - '1')
               || !(cols[i%9] & 1 << inp[i] - '1')
               || !(squares[(i/27)*3 + (i%9)/3] & 1 << inp[i] - '1'))
            {
                //if used, end program
                cout << "\nYou entered the following puzzle:\n";
                print();
                cout << "\ninvalid puzzle";
                delete[] inp;
                return 1;
            }

            //digit is free. mark it as such
            rows[i/9] &= ~(1 << inp[i] - '1');
            cols[i%9] &= ~(1 << inp[i] - '1');
            squares[(i/27)*3 + (i%9)/3] &= ~(1 << inp[i] - '1');
        }
    }

    delete[] inp;
    cout << "\nYou entered the following puzzle:\n";
    print();

    //start with -1 depth because it gets incremented on entering. monkey code
    unsigned char res = solveSquare(-1);

    if(res == RES_BADNUM)
    {
        cout << "the entered puzzle is unsolvable";
        return 0;
    }

    //print the result
    cout << "\nSolution:\n";
    print();

//    cout << "res: " << (unsigned short) res << "\n\n";
//    cout << "\n\n";
//    for(int i = 0; i < 9; ++i)
//    {
//        cout << "0x" << hex << squares[i];
//        cout << '\n';
//    }

    return 0;
}
