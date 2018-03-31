#include "stdafx.h"
#include "Delegate.h"
using namespace std;

class A
{
	private:
		int value;

	public:
		A(int value)
		{
			this->value = value;
		}

		void FooDynamic(int i, float i2)
		{
			// do stuff
		}

		static void FooStatic(int i, float i2)
		{
			// do stuff
		}
};

void TestDelegate()
{
	#define StaticMethod (Delegate<void, int, float>::BindStatic::Method)&A::FooStatic
	#define DynamicMethod (Delegate<void, int, float>::BindDynamic<A>::Method)&A::FooDynamic

	// create delegate
	auto d = Delegate<void, int, float>();

	// static method
	d.Add(StaticMethod);

	// dynamic method
	auto a = new A(1);
	auto a2 = new A(2);
	d.Add<A>(DynamicMethod, a);
	d.Add<A>(DynamicMethod, a2);

	// this will invoke FooStatic, a.Foo, a2.Foo in order
	d.Invoke(123, 1.4f);

	// manually remove bind if wanted
	d.Remove(StaticMethod);
	d.Remove<A>(DynamicMethod, a);

	// only a2 will invoke here
	d.Invoke(123, 1.4f);
}