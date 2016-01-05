#ifdef WIN32
#include <Windows.h>
template<typename T>
class ThreadLocal 
{
public:
	ThreadLocal()
	{
		tlsKey_ = TlsAlloc();
		assert(tlsKey_ != TLS_OUT_OF_INDEXES);
	}

	~ThreadLocal()
	{
		LPVOID p = TlsGetValue(tlsKey_);
		if (p != NULL)
		{
			T *obj = static_cast<T*>(p);
			delete obj;
		}

		TlsFree(tlsKey_);
	}

public:
	T operator()()
	{
		return *get();
	}

	T& operator*()
	{
		return *get();
	}

	T* operator->()
	{
		return get();
	}

	T* get() const
	{
		LPVOID p = TlsGetValue(tlsKey_);
		if (p == NULL)
		{
			T *obj = new T;
			TlsSetValue(tlsKey_, obj);
			return obj;
		}
		else
		{
			return static_cast<T*>(p);
		}
	}

	T* release()
	{
		T *obj = static_cast<T*>(TlsGetValue(tlsKey_));
		TlsSetValue(tlsKey_, NULL);
		return obj;
	}

	void reset(T *p = NULL)
	{
		T *obj = static_cast<T*>(TlsGetValue(tlsKey_));
		delete obj;
		TlsSetValue(tlsKey_, p);
	}

private:
	DWORD tlsKey_;
};

#else
template<typename T>
class ThreadLocal 
{
public:
	ThreadLocal()
	{
		pthread_key_create(&tlsKey_, &ThreadLocal::cleanHook);
	}

	~ThreadLocal()
	{
		T *p = get();
		if (p)
			delete p;
		pthread_key_delete(tlsKey_);
	}

public:
	T operator()()
	{
		return *get();
	}

	T& operator*()
	{
		return *get();
	}

	T* operator->()
	{
		return get();
	}

	T* get() const
	{
		T* obj = static_cast<T*>(pthread_getspecific(tlsKey_));
		if (!obj)
		{
			T* newObj = new T;
			pthread_setspecific(tlsKey_, newObj);
			obj = newObj;
		}
		return obj;
	}

	T* release()
	{
		T *obj = get();
		pthread_setspecific(tlsKey_, NULL);
		return obj;
	}

	void reset(T *p = 0)
	{
		T *obj = get();
		delete obj;
		obj = 0;
		pthread_setspecific(tlsKey_, p);
	}

private:
	static void cleanHook(void *x)
	{
		T* obj = static_cast<T*>(x);
		delete obj;
		obj = 0;
	}

private:
	pthread_key_t tlsKey_;
};
#endif