
#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <memory>

namespace glovebox {

	const auto ALWAYS = [](){ return true; };

	template <class ParamType>
	class Param : public std::deque < ParamType > {
		using funcType = std::function<ParamType()>;
		using condType = std::function<ParamType()>;

		condType cond;
		funcType func;
		std::vector<Param*> children;
		size_type capacity;

	public:
		Param() : children{} {}

		Param(const ParamType seed, const size_type capacity, const condType cond, const funcType func) :
			cond{ cond }, capacity{ capacity }, func{ func }, children{} {
			push_back(seed);
		}

		Param(const ParamType seed, const condType cond, const funcType func) :
			cond{ cond }, capacity{ capacity }, func{ func }, children{} {
			push_back(seed);
		}

		template<class ChildType>
		void notify(ChildType&& child) { children.push_back(child); }

		void setFunction(const funcType func) { this->func = func; }

		void setCondition(const condType cond) { this->cond = cond; }

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
		ParamType operator()() const {
			return back();
		}
	};

	template <class ParamId, class ParamType>
	class System {
		
		using funcType = std::function<ParamType()>;
		using condType = std::function<ParamType()>;

		ParamId root;
		std::map<ParamId, std::unique_ptr<Param<ParamType>>> params;

	public:

		auto operator() (const ParamId param_id) {
			return (*params.at(param_id))();
		}

		auto param(const ParamId param_id) {
			return params.at(param_id).get();
		}

		void update() {
			param(root)->update();
		}

		void setRoot(const ParamId param_id) { root = param_id; }

		void define(const ParamId param_id, const funcType func) {
			auto param = std::make_unique<Param<ParamType>>(Param<ParamType>(0, ALWAYS, func));
			params[param_id] = std::move(param);
		}

		void link(const ParamId src, const ParamId dest) {
			param(src)->notify(param(dest));
		}

	};
}
