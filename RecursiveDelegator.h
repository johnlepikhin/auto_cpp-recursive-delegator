// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef RECURSIVEDELEGATOR_H_
#define RECURSIVEDELEGATOR_H_

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace RecursiveDelegator {

template<typename PARENT, typename THIS>
class Processor
{
public:
	Processor() {};

	virtual ~Processor()
	{
		for (auto i : Followers) {
			delete i;
		}
	};

	typedef Processor<THIS, void> FollowerType;

	std::vector<FollowerType *> Followers;

	void AddFollower(FollowerType *follower)
	{
		Followers.push_back(follower);
	};

	virtual std::shared_ptr<THIS> Process(const std::shared_ptr<PARENT> &parent) = 0;

	virtual bool BeforeRecursionHook(const std::shared_ptr<THIS> &got) { return (true); };
	virtual bool AfterRecursionHook(const std::shared_ptr<THIS> &got, const std::exception *exn, bool found) { if (exn) { throw (*exn); } else { return (found); }; };

	Processor<PARENT, void> *AsFollower() { return(reinterpret_cast<Processor<PARENT, void> *>(this)); }

	bool Recursive (std::shared_ptr<PARENT> parent)
	{
		std::shared_ptr<THIS> got = Process(parent);
		if (got) {
			try {
				if (BeforeRecursionHook(got)) {
					bool found = false;
					for (auto f : Followers) {
						found = f->Recursive(got);
						if (found)
							break;
					}
					return (AfterRecursionHook(got, NULL, found));
				} else {
					return (AfterRecursionHook(got, NULL, false));
				}
			} catch (const std::exception &exn) {
				return (AfterRecursionHook(got, &exn, false));
			}
		}

		return (false);
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
