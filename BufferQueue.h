#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <omp.h>
#include <stack>
using namespace std;

// Definition of BufferQueue class

template <typename T>
class BufferQueue
{
	// Inpput-output:
	template <typename Z>
	friend istream&	operator>> (istream& in,  BufferQueue<Z>& obj);
	template <typename Z>
	friend ostream& operator<< (ostream& out, BufferQueue<Z>& obj);

	public	:

		// Attributes:
		T* inside;
		int length;
		int head, tail;

		// Constructors:
		BufferQueue<T> ();
		BufferQueue<T> (int t);
		BufferQueue<T> (const BufferQueue<T>& other);

		// Destructors:
		~BufferQueue<T> ();

		// Methods:
		T pop();
		void push (T data);
};


// Implementation
// Input-Output overload:

template <typename T>
istream& operator>> (istream& in,  BufferQueue<T>& obj)
{
	// .../...
	return	in;
}

template <typename T>
ostream& operator<< (ostream& out, BufferQueue<T>& obj)
{	
	out<<"Inside; Length; Head; Tail"<<endl;
	out<<obj.inside<<"; "<<obj.length<<"; "<<obj.head<<"; "<<obj.tail<<endl;
	return	out;
}

// Constructors:

int debug = 0;
#define	say	if(debug) cout

//If no size is specified we make the size equal to 3 by default
template <typename T>
BufferQueue<T>::BufferQueue ()
{
	length = 3;
	say <<	" BufferQueue() \n" ;
}

template <typename T>
BufferQueue<T>::BufferQueue	(int t)
{
	inside = new T[t];
	length = t;
	head = 1;
	tail = 0;
	say <<" BufferQueue(int)\n";
}

template <typename T>
BufferQueue<T>::BufferQueue	(const BufferQueue<T>& other)
{
	say <<	" BufferQueue(BufferQueue<T>&) \n";
}

// Destructor:
	
template <typename T>
BufferQueue<T>::~BufferQueue()
{
	say <<	" ~BufferQueue() \n";	
}

// Methods:
/*
	Method to add and to get the last element
*/
	
template	<typename T>
T BufferQueue<T>::pop()
{
	T tmp = inside[head];
	inside[head] = 0;
	head = (head + 1) % length;
	return	tmp;
}

template	<typename T>
void BufferQueue<T>::push (T data)
{
	tail = (tail + 1) % length;
	inside[tail % length] = data;
}
