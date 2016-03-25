#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <functional>
#include <memory>

namespace glovebox {

	auto ALWAYS = [](){return true; };

	template <class T>
	class Param : public std::deque < T > {

		std::function<bool()> cond;
		std::function<T()> func;
		std::vector<Param*> children;
		size_type capacity;

	public:
		Param() : children(){}

		Param(T seed, size_type capacity, std::function<bool()> cond, std::function<T()> func) :
			cond(cond), capacity(capacity), func(func), children() {
			push_back(seed);
		}
		
		Param(T seed, std::function<bool()> cond, std::function<T()> func) :
			cond(cond), capacity(max_size()), func(func), children() {
			push_back(seed);
		}


		void notify(Param *child) {
			children.push_back(child);
		}

		void update() {
			if (cond()) {
				push_back(func());
				if (size() > capacity) {
					pop_front();
				}
				for (auto child : children) {
					child->update();
				}
			}
		}
		T operator()() {
			return back();
		}
	};
}
