#pragma once
#include <vector>

namespace std
{
	template<class Return, typename ...Types>
	class Delegate
	{
		public:
			class BindBase
			{
				public:
					virtual void Invoke(Types... args) {}
			};

			class BindStatic : public BindBase
			{
				public:
					typedef Return*(*Method)(Types... args);
					Method method;

				public:
					BindStatic(Method method)
					{
						this->method = method;
					}

					virtual void Invoke(Types... args) override
					{
						method(args...);
					}
			};

			template<class T>
			class BindDynamic : public BindBase
			{
				public:
					typedef Return*(__thiscall T::*Method)(Types... args);
					Method method;
					T* instance;

				public:
					BindDynamic(Method method, T* instance)
					{
						this->method = method;
						this->instance = instance;
					}

					virtual void Invoke(Types... args) override
					{
						(instance->*(method))(args...);
					}
			};

		private:
			std::vector<BindBase*> binds;

		public:
			Delegate()
			{
				binds = std::vector<BindBase*>();
			}

			void RemoveAll()
			{
				for (int i = 0; i != binds.size(); ++i) delete binds[i];
				binds.clear();
			}

			~Delegate()
			{
				RemoveAll();
			}

			void Add(typename BindStatic::Method method)
			{
				auto bind = new BindStatic(method);
				binds.push_back(bind);
			}

			template<class T>
			void Add(typename BindDynamic<T>::Method method, T* instance)
			{
				auto bind = new BindDynamic<T>(method, instance);
				binds.push_back(bind);
			}

			void Remove(typename BindStatic::Method method)
			{
				REMOVE_NEXT:;
				int found = -1, foundCount = 0;
				for (int i = 0; i != binds.size(); ++i)
				{
					auto bindType = (BindStatic*)binds[i];
					if (bindType->method == method)
					{
						found = i;
						++foundCount;
					}
				}

				if (found != -1)
				{
					delete binds[found];
					binds.erase(binds.begin() + found);
					if (foundCount != 1) goto REMOVE_NEXT;
				}
			}

			template<class T>
			void Remove(typename BindDynamic<T>::Method method, T* instance)
			{
				REMOVE_NEXT:;
				int found = -1, foundCount = 0;
				for (int i = 0; i != binds.size(); ++i)
				{
					auto bindType = (BindDynamic<T>*)binds[i];
					if (bindType->method == method && bindType->instance == instance)
					{
						found = i;
						++foundCount;
					}
				}

				if (found != -1)
				{
					delete binds[found];
					binds.erase(binds.begin() + found);
					if (foundCount != 1) goto REMOVE_NEXT;
				}
			}

			void Invoke(Types... args)
			{
				for (int i = 0; i != binds.size(); ++i) binds[i]->Invoke(args...);
			}
	};
}