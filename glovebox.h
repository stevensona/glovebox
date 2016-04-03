#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <atomic>

namespace glovebox {

	const auto ALWAYS = [](){ return true; };

	template <class ParamId, class ParamType>
	class System {

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
				value{ seed }, cond{ cond }, func{ func }, children{} {}

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
			//weak_ptr not used because System uses unique_ptr, not shared_ptr
			using paramPtr = Parameter<ParamType>*;
			using paramData = std::vector<ParamType>;
			using dataMap = std::map<paramPtr, paramData>;

			std::vector<paramPtr> probes;
			dataMap data;

		public:
			void monitor(paramPtr probe) {
				probes.emplace_back(probe);
				data[probe] = {};
			}
			void capture() {
				for (auto p : probes) {
					data[p].emplace_back((*p)());
				}
			}
			void display() {
				for (auto p : probes) {
					for (auto d : data[p]) {
						std::cout << d << ' ';
					}
					std::cout << std::endl;
				}
			}
		};

		using funcType = std::function<ParamType()>;
		using condType = std::function<ParamType()>;
		using paramPtr = std::unique_ptr<Parameter<ParamType>>;
		using paramMap = std::map<ParamId, paramPtr>;

		ParamId root;
		paramMap params;
		Recorder<ParamType> recorder;
		std::atomic_bool capture;
		std::thread capture_thread;

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
		
		template<class T>
		void startCapture(const T period) {
			stopCapture();
			capture = true;
			capture_thread = std::thread([this, period] {
				while (capture) {
					recorder.capture();
					std::this_thread::sleep_for(period);
				}
			});
		}

		void stopCapture() {
			capture = false;
			if (capture_thread.joinable()) {
				capture_thread.join();
			}
		}

		void display() {
			recorder.display();
		}

		void setRoot(const ParamId param_id) {
			root = param_id; 
		}

		void define(const ParamId param_id, const funcType func) {
			//make_unique not used because Parameter's copy constructor is deleted
			paramPtr p{ new Parameter<ParamType>{0, ALWAYS, func} };
			params[param_id] = std::move(p);
			recorder.monitor(param(param_id));
		}

		void link(const ParamId src, const ParamId dest) {
			param(src)->notify(param(dest));
		}

	};
}
