/**
 * @file singleton.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __SINGLETON_HXX__
#define __SINGLETON_HXX__

#include <memory>
#include <mutex>
#include <atomic>
#include "default_config.hxx"
NAMESPACE_PREFIX

/**
 * @brief single ton
 * 
 * @tparam T 
 */
template<typename T>
class CSingleTon
{
	private:
		class CDeletor
		{
			public:
				CDeletor()=default;
				~CDeletor(){
					T* temp = pInstance.load();
					if(temp != nullptr){
						delete temp;
						pInstance.store(nullptr);
					}
				}
		};
		static CDeletor deletor;
		static std::atomic<T*> pInstance;
		static std::mutex	mtx;
		CSingleTon() = delete;
		~CSingleTon() = delete;
		CSingleTon(const CSingleTon&) = delete;
		CSingleTon& operator=(const CSingleTon&) = delete;
	public:
		template<typename ...arguments>
		static T* GetInstance(arguments... args){
			deletor = deletor;
			T* temp = pInstance.load();
			if (temp == nullptr) {
				std::lock_guard<std::mutex> lock{ mtx };    //double check lock pattern
				temp = pInstance.load();
				if (temp == nullptr) {
					pInstance.store( new T(std::forward<arguments>(args)...));
				}
			}
			return pInstance.load();
		}

};
template<typename T>
typename CSingleTon<T>::CDeletor CSingleTon<T>::deletor;
template<typename T>
std::atomic<T*> CSingleTon<T>::pInstance(nullptr);
template<typename T>
std::mutex			CSingleTon<T>::mtx;

NAMESPACE_SUBFIX
#endif  ///__SINGLETON_HXX__