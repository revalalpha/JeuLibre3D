#pragma once 
template<typename T>
struct DataDirty
{
	bool isDirty = false;
	T data = T();
};


template<typename T, size_t targetSize>
struct PadData;



template<typename T, size_t targetSize> requires(targetSize > sizeof(T))
struct PadData<T, targetSize>
{

	PadData() : T(), tabPad() {}
	PadData(const T& copy) : val(copy), tabPad() {}
	PadData(T&& move) noexcept : val(std::move(move)), tabPad() {}
	PadData& operator=(const T& data)
	{
		val = data;
		return *this;
	}
	PadData& operator=(T&& data) noexcept
	{
		val = std::move(data);
		return *this;
	}
	operator T& ()
	{
		return val;
	}
	operator const T& () const
	{
		return val;
	}
	T* operator->()
	{
		return &val;
	}
	const T* operator->() const
	{
		return &val;
	}
	T val;
private:
	char tabPad[targetSize - sizeof(T)];
};

template<typename T, size_t targetSize> requires(targetSize == sizeof(T))
struct PadData<T, targetSize>
{
	PadData() : T() {}
	PadData(const T& copy) : val(copy) {}
	PadData(T&& move) noexcept : val(std::move(move)) {}
	PadData& operator=(const T& data)
	{
		val = data;
		return *this;
	}
	PadData& operator=(T&& data) noexcept
	{
		val = std::move(data);
		return *this;
	}
	operator T& ()
	{
		return val;
	}
	operator const T& () const
	{
		return val;
	}
	T* operator->()
	{
		return &val;
	}
	const T* operator->() const
	{
		return &val;
	}
	T val;
};