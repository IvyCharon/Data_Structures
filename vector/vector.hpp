#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
public:
	T *data;
	int nowsize;
	int maxsize;
	void expand_space()
	{
		T* tmp = data;
		maxsize *= 2;
		data = (T*)(operator new(sizeof(T) * maxsize));
		for (int i = 0;i < nowsize;++i)
			memcpy(data + i,tmp + i,sizeof(T));
		for (T* p = tmp; p != tmp + nowsize;++p)
			p -> ~T();
		operator delete(tmp);
	}
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator 
	{
		friend class vector;
	private:
		T *pos;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		friend class vector;
		//iterator end();
		iterator(T *obj = NULL) { pos = obj; }
		iterator operator+(const int &n) const 
		{
			iterator tmp(pos + n);
			return tmp;
		}
		iterator operator-(const int &n) const 
		{
			iterator tmp(pos - n);
			return tmp;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invalid_iterator.
		int operator-(const iterator &rhs) const 
		{
			bool jud = 0;
			iterator tmp = *this;
			while(tmp != iterator(data) && (!jud))
			{
				if(rhs == tmp) jud = 1;
				tmp --;
			}
			tmp = *this;
			while(tmp != iterator(data + nowsize - 1) && (!jud))
			{
				if(rhs == tmp) jud = 1;
				tmp ++;
			}
			if(! jud)throw invalid_iterator();
			return (*this) - rhs;
		}
		iterator& operator+=(const int &n) 
		{
			pos += n;
			return *this;
		}
		iterator& operator-=(const int &n) 
		{
			pos -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) 
		{
			iterator tmp(++ pos);
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() 
		{
			pos ++;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) 
		{
			iterator tmp(-- pos);
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() 
		{
			pos --;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
			return *pos;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const 
		{
			return (pos == rhs.pos);
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return (pos == rhs.pos);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const 
		{
			return (pos != rhs.pos);
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return (pos != rhs.pos);
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator 
	{
		T *pos;
	public:
		const_iterator(T *obj = NULL) : pos(obj) {}
		friend class iterator;
		const_iterator end();
		int operator-(const const_iterator &rhs) const 
		{
			bool jud = 0;
			const_iterator tmp = *this;
			while(tmp != const_iterator(data) && (!jud))
			{
				if(rhs == tmp) jud = 1;
				tmp --;
			}
			tmp = *this;
			while(tmp != const_iterator(data + nowsize - 1) && (!jud))
			{
				if(rhs == tmp) jud = 1;
				tmp ++;
			}
			if(! jud)throw invalid_iterator();
			return (*this) - rhs;
		}
		const_iterator operator+(const int &n) const 
		{
			const_iterator tmp(pos + n);
			return tmp;
		}
		const_iterator operator-(const int &n) const 
		{
			const_iterator tmp(pos - n);
			return tmp;
		}
		const_iterator& operator+=(const int &n) 
		{
			pos += n;
			return *this;
		}
		const_iterator& operator-=(const int &n) 
		{
			pos -= n;
			return *this;
		}
		const_iterator operator++(int) 
		{
			const_iterator tmp(++ pos);
			return tmp;
		}
		const_iterator& operator++() 
		{
			pos ++;
			return *this;
		}
		const_iterator operator--(int) 
		{
			const_iterator tmp = (-- pos);
			return tmp;
		}
		const_iterator& operator--() 
		{
			pos --;
			return *this;
		}
		T& operator*() const
		{
			return *pos;
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return (pos == rhs.pos);
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return (pos != rhs.pos);
		}
		bool operator==(const iterator &rhs) const 
		{
			return (pos == rhs.pos);
		}
		bool operator!=(const iterator &rhs) const 
		{
			return (pos != rhs.pos);
		}
	};
	/**
	 * TODO Constructs
	 * Atleast two: default constructor, copy constructor
	 */
	vector() 
	{
		nowsize = 0;
		maxsize = 10;
		data = (T*)(operator new(sizeof(T) * maxsize));
	}
	vector(const vector &other) 
	{
		maxsize = other.maxsize;
		data = (T *)(operator new(sizeof(T) * maxsize));
		nowsize = other.nowsize;
		for(int i = 0;i < other.nowsize;++ i)
			memcpy(data + i,other.data + i,sizeof(T));
		//	data[i] =  other[i];
	}
	/**
	 * TODO Destructor
	 */
	~vector() 
	{
		for(T* tmp = data;tmp < data + nowsize;++ tmp)
			tmp -> ~T();
		operator delete(data);
		nowsize = 0;	
		//printf("daysfga\n");		
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) 
	{
		if(this == &other) return *this;
		
		while(maxsize <= other.nowsize)
			expand_space();
		nowsize = other.nowsize;
		for(int i = 0;i < nowsize;++ i)
			memcpy(data + i,other.data + i,sizeof(T));
		//	data[i] = other[i];
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) 
	{
		if(pos < 0 || pos >= nowsize) throw index_out_of_bound();
		return data[pos];
	}
	const T & at(const size_t &pos) const 
	{
		if(pos < 0 || pos >= nowsize) throw index_out_of_bound();
		return data[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) 
	{
		if(pos < 0 || pos >= nowsize) throw index_out_of_bound();
		return data[pos];
	}
	const T & operator[](const size_t &pos) const //const对象只能调用这个
	{
		if(pos < 0 || pos >= nowsize) throw index_out_of_bound();
		return data[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const 
	{
		if(nowsize == 0) throw container_is_empty();
		return data[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const 
	{
		if(nowsize == 0)throw container_is_empty();
		return data[nowsize - 1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() { return iterator(data); }
	const_iterator cbegin() const { return const_iterator(data); }
	/**
	 * returns an iterator to the end.
	 */
	iterator end() { return iterator(data + nowsize); }
	const_iterator cend() const { return const_iterator(data + nowsize); }
	/**
	 * checks whether the container is empty
	 */
	bool empty() const { return (nowsize == 0); }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return nowsize; }
	/**
	 * clears the contents
	 */
	void clear() 
	{
		//for(T* p = data; p < data + nowsize; ++ p)
		//	p -> ~T();
		//operator delete(data);
		//maxsize = 0;
		//data = (T*)(opertor new(sizeof(T) * maxsize));
		nowsize = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator posi, const T &value) 
	{
		int t = posi.pos - data;
		if(posi == end())
		{
			if(nowsize >= maxsize) expand_space();
			nowsize ++;
			new(data + nowsize - 1) T(value);
			return iterator(data + nowsize - 1);
		}
		if(nowsize >= maxsize) expand_space();
		nowsize ++;
		for(int i = nowsize - 1;i > t;-- i)
			memcpy(data + i,data + i - 1,sizeof(T));
		//	data[i] = data[i - 1];
		new(data + t) T(value);
		return iterator(data + t);
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) 
	{
		if(ind > nowsize) throw index_out_of_bound();
		if(nowsize >= maxsize) expand_space();
		nowsize ++;
		for(int i = nowsize - 1;i > ind;-- i)
			memcpy(data + i,data + i - 1,sizeof(T));
		new(data + ind) T(value);
		return iterator(data + ind);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) 
	{
		int t = pos.pos - data;
		for(int i = t;i < nowsize;++ i)
			memcpy(data + i,data + i + 1,sizeof(T));
		nowsize --;
		if(t == nowsize)return end();
		else return(data + t);
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) 
	{
		if(ind >= nowsize) throw index_out_of_bound();
		for(int i = ind;i < nowsize;++ i)
			memcpy(data + i,data + i + 1,sizeof(T));
		//	data[i] = data[i + 1];
		nowsize --;
		return iterator(data + ind);
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) 
	{
		if(nowsize == maxsize)
			expand_space();
		nowsize ++;
		new(data + nowsize - 1) T(value);
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() 
	{
		if(nowsize == 0)throw container_is_empty();
		nowsize --;
	}
};


}

#endif
