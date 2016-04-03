#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <memory>
#include <atomic>

namespace mission {

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

			void setValue(const ParamType value) {
				this->value = value;
			}

			void setFunction(const funcType func) {
				this->func = func;
			}

			void setCondition(const condType cond) {
				this->cond = cond;
			}

			void update() {
				//If Parameter's update condition is met, then store new value
				if (cond()) {
					value = func();
					//Notify dependent parameters of update
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
			using paramData = std::deque<ParamType>;
			using dataMap = std::map<paramPtr, paramData>;

			std::vector<paramPtr> probes;
			dataMap data;
			size_t memory;

		public:
			Recorder(const size_t memory) :memory{ memory } {}
			void monitor(paramPtr probe) {
				probes.push_back(probe);
				data[probe] = {};
			}

			//Save a snapshot of currently monitored values
			void capture() {
				for (auto p : probes) {
					data[p].emplace_back((*p)());
					//Remove oldest element if necessary to maintain size below the specified capacity
					if (data[p].size() > memory) {
						data[p].pop_front();
					}
				}
			}

			//Displays the contents of recorder memory to std::cout
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
		Recorder<ParamType> recorder{ 64 };
		std::atomic_bool capture;
		std::thread capture_thread;

		//Return Parameter* with given identifier
		auto param(const ParamId param_id) {
			return params.at(param_id).get();
		}

	public:
		void set(const ParamId param_id, const ParamType value) {
			param(param_id)->setValue(value);
		}
		//Returns the current value of specified parameter
		auto operator() (const ParamId param_id) const {
			return (*params.at(param_id))();
		}

		//Update the root Parameter (Presumably kicks off a chain of updates)
		void update() {
			param(root)->update();
		}
		
		//Initiate recorder at a given period (1/Frequency) to capture data
		template<class T>
		void startCapture(const T period) {
			stopCapture();
			capture = true;
			//Begin capturing data in seperate thread
			capture_thread = std::thread([this, period] {
				while (capture) {
					recorder.capture();
					std::this_thread::sleep_for(period);
				}
			});
		}

		//Stop the recorder from capturing system parameters
		void stopCapture() {
			capture = false;
			if (capture_thread.joinable()) {
				//Wait for capture thread to finish
				capture_thread.join();
			}
		}

		//Displays the contents of the recorder. For debugging only
		void display() {
			recorder.display();
		}

		//Identifies in the system what will be used as the "root" parameter
		//which will be updated every time the System updates
		void setRoot(const ParamId param_id) {
			root = param_id; 
		}

		//Defines a system parameter's update function and applies a recorder
		//probe to its value
		void define(const ParamId param_id, const funcType func) {
			//make_unique not used because Parameter's copy constructor is deleted
			paramPtr p{ new Parameter<ParamType>{0, ALWAYS, func} };
			params[param_id] = std::move(p);
			recorder.monitor(param(param_id));
		}

		//Establish a dependant relationship between two different parameters
		void link(const ParamId src, const ParamId dest) {
			param(src)->notify(param(dest));
		}

	};
}
