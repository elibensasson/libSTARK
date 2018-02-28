#ifndef  __Sequence_HPP
#define  __Sequence_HPP

#include <stdint.h>

namespace libstark {

/**
 * @class Sequence
 * @brief An interface class for sequences for some type T
 */
template <class T>
class Sequence {
public:
	typedef uint64_t index_t;
	/**
	 * @brief   A mapping of integers to T
	 * @param   index an integer
	 * @return  T element
	 */
	virtual T getElementByIndex(index_t index)const = 0;

	virtual ~Sequence(){};
};

/**
 * @class LazyVector
 * @brief An interface class for lazy evaluation vector for some type T
 */
template <class T>
class LazyVector : public Sequence<T> {
    public:
        virtual typename Sequence<T>::index_t size()const = 0;    
};


} // namespace libstark 

#endif   // __Sequence_HPP
