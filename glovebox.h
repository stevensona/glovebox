
#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <memory>
#include <atomic>

namespace glovebox {

	const auto ALWAYS = [](){ return true; };


	template <class ParamType>
	class Parameter {
		using funcType = std::function<ParamType()>;
		using condType = std::function<ParamType()>;

		std::atomic<ParamType> value;
		condType cond;
		funcType func;
		std::vector<Parameter*> children;

	public:
		Parameter() : children{} {}

		Parameter(const ParamType seed, const condType cond, const funcType func) :
			value{ seed }, cond { cond }, func{ func }, children{} {}

		Parameter(const Parameter&) = delete;
		Parameter& operator=(const Parameter&) = delete;

		template<class ChildType>
		void notify(ChildType&& child) {
			children.emplace_back(child);
		}

		void setFunction(const funcType func) {
			this->func = func; 
		}

		void setCondition(const condType cond) { 
			this->cond = cond; 
		}

		void update() {
			if (cond()) {
				value = func();
				for (const auto child : children) {
					child->update();
				}
			}
		}
		auto operator()() {
			return value.load();
		}
	};

	template <class ParamType>
	class Recorder {
		using paramPtr = std::weak_ptr<Parameter<ParamType>>;

		std::vector<paramPtr> probes;
		std::vector<ParamType> data;

	public:
		template<class ProbeType>
		void monitor(ProbeType &&probe) {
			probes.emplace_back(probe);
		}
		void capture() {
			for (auto p : probes) {

			}
		}
	};


	template <class ParamId, class ParamType>
	class System {
		
		using funcType = std::function<ParamType()>;
		using condType = std::function<ParamType()>;
		using paramPtr = std::unique_ptr<Parameter<ParamType>>;
		using paramMap = std::map<ParamId, paramPtr>;

		ParamId root;
		paramMap params;
		Recorder<ParamType> recorder;

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

		void setRoot(const ParamId param_id) {
			root = param_id; 
		}

		void define(const ParamId param_id, const funcType func) {
			//make_unique not used because Parameter's copy constructor is deleted
			paramPtr param{ new Parameter<ParamType>{0, ALWAYS, func} };
			params[param_id] = std::move(param);
		}

		void link(const ParamId src, const ParamId dest) {
			param(src)->notify(param(dest));
		}

	};
}
