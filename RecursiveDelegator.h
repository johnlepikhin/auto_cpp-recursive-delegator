
#ifndef RECURSIVEDELEGATOR_H_
#define RECURSIVEDELEGATOR_H_

#include <string>
#include <vector>
#include <iostream>

namespace RecursiveDelegator {

template<typename PARENT, typename THIS>
class Processor
{
public:
	typedef Processor<THIS, void> FollowerType;

	std::vector<FollowerType *> Followers;

	void AddFollower(FollowerType *follower)
	{
		Followers.push_back(follower);
	}

	virtual THIS *Process(PARENT *parent) = 0;

	virtual void BeforeRecursionHook(THIS *got) {}
	virtual void AfterRecursionHook(THIS *got) {}

	void Recursive (PARENT *parent)
	{
		THIS *got = Process(parent);
		if (got) {
			try {
				BeforeRecursionHook(got);
				for (auto f : Followers) {
					f->Recursive(got);
				}
				AfterRecursionHook(got);
			} catch (...) {
				AfterRecursionHook(got);
			}
		}
	}
};

}

// Example usage
/*
template<typename PARENT, typename THIS>
class Base : public Processor<PARENT, THIS>
{
	void AfterRecursionHook(THIS *got)
	{
		delete got;
	}
};

class StringOfInt : public Base<int, std::string>
{
	public:

		std::string *Process(int *p)
		{
			std::cout << "(StringOfInt) My parent is: " << (*p) << std::endl;
			std::string *r = new std::string("hello");
			return (r);
		}
};

class DoubleOfString : public Base<std::string, double>
{
	public:
		double *Process(std::string *p)
		{
			std::cout << "(DoubleOfString) My parent is: " << (*p) << std::endl;
			double *r = new double(3.14);
			return (r);
		}
};

class IntOfDouble : public Base<double, int>
{
	public:
		int *Process(double *p)
		{
			std::cout << "(IntOfDouble) My parent is: " << (*p) << std::endl;
			return (new int(5));
		}
};

int main ()
{
	// accepts string, generates double, calls double->any
	DoubleOfString *doubleOfString = new DoubleOfString;

	// accepts double, generates int, calls int->any
	IntOfDouble *intOfDouble = new IntOfDouble;

	// accepts int, generates string, calls string->any
	StringOfInt *stringOfInt = new StringOfInt;

	doubleOfString->AddFollower((Processor<double, void>*)intOfDouble);
	doubleOfString->AddFollower((Processor<double, void>*)intOfDouble);
	intOfDouble->AddFollower((Processor<int, void>*)stringOfInt);

	std::string *input = new std::string("input data");
	doubleOfString->Recursive(input);

	delete doubleOfString;
	delete intOfDouble;
	delete stringOfInt;
	delete input;
}
*/

#endif
