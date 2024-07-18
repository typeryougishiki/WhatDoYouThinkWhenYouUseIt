#pragma once
template <typename T>
class ThreadSafeObjPkj {
protected:
	mutable std::shared_mutex mutex;
	std::unique_ptr<T> obj;
public:
	using ValueRef = T&;
	using constValueRef = const ValueRef;
	inline void ConstAccess(const std::function<void(constValueRef)>& access) const {
		std::shared_lock<std::shared_mutex> guard{ this->mutex };
		access(*obj);
	};
	inline void NotConstAccess(const std::function<void(ValueRef)>& access) {
		std::unique_lock<std::shared_mutex> guard{ this->mutex };
		access(*obj);
	};
	inline constexpr auto operator=(const T&) = delete;
	inline constexpr auto operator=(T&&) = delete;


	inline void reset(std::unique_ptr<T>&& new_value) {
		std::unique_lock guard{ this->mutex };
		obj = std::move(new_value);
	}

	template<typename ...Args>
	inline void reconstruct(Args&& ...args) {
		std::unique_lock guard{ this->mutex };
		obj = std::make_unique<T>(std::forward<Args>(args)...);
	}
	inline ThreadSafeObjPkj(std::unique_ptr<T>&& new_value) :mutex(), obj(std::move(new_value)) {};
};
