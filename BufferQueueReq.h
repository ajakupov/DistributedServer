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

// Definition of BufferQueueReq class

template <typename T>
class BufferQueueReq
{
	// Inpput-output:
	template <typename Z>
	friend istream&	operator>> (istream& in,  BufferQueueReq<Z>& obj);
	template <typename Z>
	friend ostream& operator<< (ostream& out, BufferQueueReq<Z>& obj);

	public	:

		// Attributes:
		T** inside;
		int length;
		int head, tail;

		// Constructors:
		BufferQueueReq<T> ();
		BufferQueueReq<T> (int t);
		BufferQueueReq<T> (const BufferQueueReq<T>& other);

		// Destructors:
		~BufferQueueReq<T> ();

		// Methods:
		T* pop();
		void push (T* data);
};


// Implementation
// Input-Output overload:

template <typename T>
istream& operator>> (istream& in,  BufferQueueReq<T>& obj)
{
	// .../...
	return	in;
}

template <typename T>
ostream& operator<< (ostream& out, BufferQueueReq<T>& obj)
{	
	out<<"Inside; Length; Head; Tail"<<endl;
	out<<obj.inside<<"; "<<obj.length<<"; "<<obj.head<<"; "<<obj.tail<<endl;
	return	out;
}

// Constructors:

int debug_req = 0;
#define	say_req	if(debug_req) cout

//If no size is specified we make the size equal to 3 by default
template <typename T>
BufferQueueReq<T>::BufferQueueReq ()
{
	length = 3;
	say_req <<	" BufferQueueReq() \n" ;
}

template <typename T>
BufferQueueReq<T>::BufferQueueReq (int t)
{
	inside = new T*[t];
	length = t;
	head = 1;
	tail = 0;
	say_req <<" BufferQueueReq(int)\n";
}

template <typename T>
BufferQueueReq<T>::BufferQueueReq	(const BufferQueueReq<T>& other)
{
	say_req <<	" BufferQueueReq(BufferQueueReq<T>&) \n";
}

// Destructor:
	
template <typename T>
BufferQueueReq<T>::~BufferQueueReq()
{
	say_req <<	" ~BufferQueueReq() \n";	
}

// Methods:
/*
	Method to add and to get the last element
*/
	
template	<typename T>
T* BufferQueueReq<T>::pop()
{
	T* tmp = inside[head];
	inside[head] = 0;
	head = (head + 1) % length;
	return	tmp;
}

template	<typename T>
void BufferQueueReq<T>::push (T* data)
{
	tail = (tail + 1) % length;
	inside[tail % length] = data;
}
