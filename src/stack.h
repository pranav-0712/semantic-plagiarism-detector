/*
 * ============================================================
 *  stack.h  –  Generic Stack ADT (Array-Based)
 *
 *  A fixed-capacity stack that stores elements of any type T.
 *  Used in this project to process document tokens during the
 *  n-gram generation pipeline.
 * ============================================================
 */

#ifndef STACK_H
#define STACK_H

#include <stdexcept>
#include <string>

// Maximum number of elements the stack can hold
const int STACK_MAX = 1024;

/*
 * Stack<T>
 * --------
 * A generic, array-based Stack ADT.
 * Supports standard push / pop / peek operations in O(1) time.
 */
template <typename T>
class Stack {
private:
    T    data[STACK_MAX];   // Internal storage array
    int  topIndex;          // Index of the top element (-1 when empty)

public:
    // ----- Constructor -----
    Stack() : topIndex(-1) {}

    /*
     * push(item)
     * Adds 'item' on top of the stack.
     * Throws std::overflow_error if the stack is full.
     * Time Complexity: O(1)
     */
    void push(const T& item) {
        if (isFull())
            throw std::overflow_error("Stack Overflow: cannot push onto a full stack.");
        data[++topIndex] = item;
    }

    /*
     * pop()
     * Removes and returns the top element.
     * Throws std::underflow_error if the stack is empty.
     * Time Complexity: O(1)
     */
    T pop() {
        if (isEmpty())
            throw std::underflow_error("Stack Underflow: cannot pop from an empty stack.");
        return data[topIndex--];
    }

    /*
     * peek()
     * Returns the top element WITHOUT removing it.
     * Throws std::underflow_error if the stack is empty.
     * Time Complexity: O(1)
     */
    T peek() const {
        if (isEmpty())
            throw std::underflow_error("Stack Underflow: cannot peek at an empty stack.");
        return data[topIndex];
    }

    /*
     * isEmpty()
     * Returns true if the stack contains no elements.
     * Time Complexity: O(1)
     */
    bool isEmpty() const {
        return topIndex == -1;
    }

    /*
     * isFull()
     * Returns true if the stack has reached its maximum capacity.
     * Time Complexity: O(1)
     */
    bool isFull() const {
        return topIndex == STACK_MAX - 1;
    }

    /*
     * size()
     * Returns the current number of elements in the stack.
     * Time Complexity: O(1)
     */
    int size() const {
        return topIndex + 1;
    }

    /*
     * clear()
     * Empties the stack by resetting the top pointer.
     * Time Complexity: O(1)
     */
    void clear() {
        topIndex = -1;
    }
};

#endif // STACK_H
