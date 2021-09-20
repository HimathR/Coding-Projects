#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int stack[100]; // global var stack
int top = 0;

int add(int a, int b) { return a + b; } // addition operation
int sub(int a, int b) { return b - a; } // subtraction operation 
void push(int value) { stack[top++] = value; } // puts a value on the top of the stack
int get_size() { return top; } // gets the current stack size (how many items still inside the stack)
int pop() // removes topmost element from stack
{
    if (top == 0) // if there's no operands
    {
        printf("Not enough operands in expression\n");
        exit(EXIT_FAILURE); 
    }
    return stack[--top]; // reduce stack size
}

double calcfunc(char *cmd)
{
    char *delimeter = " "; // delimeter is a space (equation is partioned by spaces)
    double result = -1; // variable to store result
    int token_length; // to account for double digit/triple digit nums
    char *token;

    token = strtok(cmd, delimeter); // tokenising using strtok
    while (token != NULL)
    {
        token_length = 0;
        while (token[token_length])
        {
            token_length++;
        }
        if (isdigit(token[0])) // if its an operand
        {
            push(atof(&token[0]));
        }
        else if (ispunct(token[0])) // if its an operator 
        {
            if (get_size() < 2) // check the expression is valid
            {
                printf("Error\n");
                return -1;
            }
            else
            {
                double operand1, operand2;
                operand1 = pop();
                operand2 = pop(); // pop two top most operands
                if (token[0] == '+') // if current token is a + operator
                {
                    // add the operands
                    result = add(operand1, operand2);
                }
                if (token[0] == '-') // same process but with subtraction
                {
                    result = sub(operand1, operand2);
                }
                push(result); // add that to the result
            }
        }
        token = strtok(NULL, delimeter);
    }
    // continue until only the final number in the stack is left, which is the result
    return pop();
}
