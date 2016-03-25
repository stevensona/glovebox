#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <memory>

namespace glovebox {

	auto ALWAYS = [](){return true; };

	template <class T>
	class Param : public std::vector < T > {

		std::function<bool()> cond;
		std::function<T()> func;
		std::vector<Param*> children;

	public:
		Param() : children(){}

		Param(T seed, std::function<bool()> cond, std::function<T()> func) :
			cond(cond), func(func), children() {
			push_back(seed);
		}

		void notify(Param *child) {
			children.push_back(child);
		}

		void update() {
			if (cond()) {
				push_back(func());
				for (auto child : children) {
					child->update();
				}
			}
		}
	};
}
