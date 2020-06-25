#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>

namespace sjtu { 

template<class T>
class deque 
{
	struct block;
	struct node
    {
        T data;
        node * next = NULL, *pre = NULL;
		block * pos = NULL;
		node(T d, node *n, node *pr, block *po) : data(d), next(n), pre(pr), pos(po) {}
    };
	struct block
    {
        node * head = NULL;//此head不存数据
		block * next = NULL, *pre = NULL;
        int len = 0;
		block() : head(NULL), next(NULL), pre(NULL), len(0) {}
		block(node *h, block *n, block* p, int l) : head(h), next(n), pre(p), len(l) {}
    };
	int num_of_block, num_of_data, maxlen;
	block * block_head;//此head不存东西
	node * tail;

	void merge(block * &tmp1,block * &tmp2)
	{
		//printf("prooooo0\n");
		tmp1 -> len = tmp1 -> len + tmp2 -> len;
		block * tmp = tmp2 -> next;
		num_of_block --;
		node * k1 = tmp2 -> head -> next;
		node * o1 = tmp1 -> head -> next;
		while(o1 -> next != NULL)
			o1 = o1 -> next;
		k1 -> pre = o1;
		o1 -> next = k1;
		while(k1 != NULL)
		{
			k1 -> pos = tmp1;
			k1 = k1 -> next;
		}
		tmp1 -> next = tmp;
		if(tmp != NULL)
			tmp -> pre = tmp1;
		//printf("prooooo1\n");
		//operator delete((void *)tmp2 -> head, sizeof(node));
		//
		delete tmp2;
		//printf("prooooo2\n");
	}
	void split(block * &k1)
	{
		block * k2, *k3 = k1 -> next;
		int left = (k1 -> len) / 2;
		node * a = k1 -> head -> next;
		for(int i = 1;i < left;++ i)
			a = a -> next;
		node * b = a -> next;
		a -> next = NULL;
		k2 = new block;
		k2 -> next = k3; k2 -> pre = k1; k2 -> len = k1 -> len - left;
		k2 -> head = (node *)operator new(sizeof(node));
		k2 -> head -> pre = NULL; k2 -> head -> next = b; k2 -> head -> pos = k2;
		b -> pre = k2 -> head;
		if(k3 != NULL)
			k3 -> pre = k2;
		k1 -> next = k2;
		num_of_block ++;
		while(b != NULL)
		{
			b -> pos = k2;
			b = b -> next;
		}
		k1 -> len = left;
	}
	void remove(block * &k)
	{
		block * kp = k -> pre, *kn = k -> next;
		if(kp != NULL)
			kp -> next = kn;
		if(kn != NULL)
			kn -> pre = kp;
		operator delete((void*)k -> head,sizeof(node));
		delete k;
	}
	void balance()
	{
		maxlen = 512;
		block * tmp1 = block_head -> next,*tmp2 = tmp1 -> next;
		while(tmp2 != NULL)
		{
			if(tmp1 -> len == 0)
			{
				remove(tmp1);
				tmp1 = tmp2;
				if(tmp1 == NULL) break;
				tmp2 = tmp1 -> next;
				continue;
			}
			while(tmp1 -> len > maxlen)
			{
				split(tmp1);
				tmp2 = tmp1 -> next;
				if(tmp2 == NULL) break;
			}
			if(tmp1 -> len + tmp2 -> len < maxlen)
				merge(tmp1,tmp2);
			tmp1 = tmp1 -> next;
			if(tmp1 == NULL) break;
			tmp2 = tmp1 -> next;
		}
		while(tmp1 != NULL && tmp1 -> len > maxlen)
			split(tmp1);
	}
	void beg(const T &value)
	{
		block_head -> next = new block;
		block * tmp = block_head -> next;
		tmp -> pre = block_head;
		tmp -> next = NULL;
		tmp -> len = 1;
		num_of_block ++;
		num_of_data ++;
		tmp -> head = (node *)operator new(sizeof(node));
		node * tt = tmp -> head;
		tt -> pre = NULL; tt -> pos = tmp;		
		tt -> next = new node(value, NULL, tt, tmp);
	}

public:
	class const_iterator;
	class iterator 
	{
		friend class deque;
		friend class const_iterator;
	private:
		deque * base;
		node * it;
	public:
		iterator(node *i = NULL, deque *b = NULL) : it(i), base(b) {}
		iterator(const iterator &other)
		{
			base = other.base;
			it = other.it;
		}
		iterator(const_iterator &other)
		{
			base = other.base;
			it = other.it;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const 
		{
			if(n == 0) return iterator(it,base);
			if(n < 0) return (*this - (-n));
			if(it == base -> tail) throw runtime_error();
			
			int p = n;
			node *mo = it;
			block *wh = it -> pos;
			while(p > 0 && mo -> next != NULL)
			{
				p --;
				mo = mo -> next;
			}
			if(p == 0) return iterator(mo, base);
			wh = wh -> next;
			while(wh != NULL && p > wh -> len)
			{
				p -= wh -> len;
				wh = wh -> next;
			}
			if(wh == NULL && p == 1) return iterator(base -> tail, base);
			else if(wh == NULL) throw runtime_error();
			node * lo = wh -> head;
			while(p --)
				lo = lo -> next;
			return iterator(lo,base);
		}
		iterator operator-(const int &n) const 
		{
			if(n == 0) return iterator(it, base);
			if(n < 0) return (*this + (-n));
			if(*this == base -> begin()) throw runtime_error();
			int p = n;
			node *mo = it;
			if(it == base -> tail)
			{
				block *k = base -> block_head;
				while(k -> next != NULL)
					k = k -> next;
				node *m = k -> head;
				while(m -> next != NULL)
					m = m -> next;
				mo = m;
				p --;
			}
			while(p > 0 && mo -> pre != mo -> pos -> head)
			{
				p --;
				mo = mo -> pre;
			}
			if(p == 0) return iterator(mo,base);
			block * lp = mo -> pos;
			lp = lp -> pre;
			while(lp != base -> block_head && p > lp -> len)
			{
				p -= lp -> len;
				lp = lp -> pre;
			}
			if(lp == base -> block_head) throw runtime_error();
			node * lo = lp -> head;
			while(lo -> next != NULL)
				lo = lo -> next;
			while(-- p)
				lo = lo -> pre;
			return iterator(lo, base);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const 
		{
			if(base != rhs.base) throw invalid_iterator();
			node *k1 = it, *k2 = rhs.it;
			if(k1 == k2) return 0;
			int ans = 0;
			block *t1 = k1 -> pos, *t2 = k2 -> pos;
			if(it == base -> tail)
			{
				ans = 1;
				block * blo_t = base -> block_head;
				while(blo_t -> next != NULL)
					blo_t = blo_t -> next;
				k1 = blo_t -> head;
				while (k1 -> next != NULL)
					k1 = k1 -> next;
				t1 = blo_t;
			}
			else if(rhs.it == rhs.base -> tail)
				return -(rhs - *this);
			
			while(t2 != NULL && t2 != t1)
				t2 = t2 -> next;
			if(t2 == NULL)
				return -(rhs - *this);

			if(k1 -> pos == k2 -> pos)
			{
				int p = 0;
				while(k2 != NULL && k2 != k1)
				{
					p ++;
					k2 = k2 -> next;
				}
				if(k2 == NULL) return -(rhs - *this);
				else return (p + ans);
			}
			else
			{
				int al = 0;
				t1 = k1 -> pos; t2 = k2 -> pos;
				while(t2 != NULL && t2 != t1)
				{
					t2 = t2 -> next;
					al += t2 -> len;
				}
				al -= t2 -> len;
				while(k2 != NULL)
				{
					k2 = k2 -> next;
					al ++;
				}
				al --;
				node *k3 = t1 -> head;
				while(k3 != k1)
				{
					k3 = k3 -> next;
					al ++;
				}
				return (al + ans);	
			}
		}

		iterator& operator+=(const int &n) 
		{
			*this = *this + n;
			return *this;
		}
		iterator& operator-=(const int &n) 
		{
			*this = *this - n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) 
		{
			if(it == base -> tail) throw invalid_iterator();
			iterator tmp = *this;
			*this = *this + 1;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() 
		{
			if(it == base -> tail) throw invalid_iterator();
			*this = *this + 1;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) 
		{
			if(it == base -> block_head -> next -> head -> next) throw invalid_iterator();
			iterator tmp = *this;
			*this = *this - 1;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() 
		{
			if(it == base -> block_head -> next -> head -> next) throw invalid_iterator();
			*this = *this - 1;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const 
		{
			if(it == base -> tail) throw invalid_iterator();
			return (it -> data);
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept 
		{
			if(it == base -> tail) throw invalid_iterator();
			return &(it -> data);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const 
		{
			return (it == rhs.it);
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return (it == rhs.it);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const 
		{
			return (it != rhs.it);
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return (it != rhs.it);
		}
	};
	class const_iterator 
	{
		friend class deque;
		friend class iterator;
	private:
		const deque * base;
		node * it;
	public:
		const_iterator(){}
		const_iterator(node *i,const deque *b) : it(i), base(b) {}
		const_iterator(const iterator &other)
		{
			base = other.base;
			it = other.it;
		}
		const_iterator(const const_iterator &other)
		{
			base = other.base;
			it = other.it;
		}
		const_iterator operator+(const int &n) const 
		{
			if(n == 0) return const_iterator(it,base);
			if(n < 0) return (*this - (-n));
			if(it == base -> tail) throw runtime_error();
			int p = n;
			node *mo = it;
			block *wh = it -> pos;
			while(p > 0 && mo -> next != NULL)
			{
				p --;
				mo = mo -> next;
			}
			if(p == 0) return const_iterator(mo, base);
			wh = wh -> next;
			while(wh != NULL && p > wh -> len)
			{
				p -= wh -> len;
				wh = wh -> next;
			}
			if(wh == NULL && p == 1) return const_iterator(base -> tail, base);
			else if(wh == NULL) throw runtime_error();
			node * lo = wh -> head -> next;
			while(-- p)
				lo = lo -> next;
			return const_iterator(lo,base);
		}
		const_iterator operator-(const int &n) const 
		{
			if(n == 0) return const_iterator(it, base);
			if(n < 0) return (*this + (-n));
			if(*this == base -> cbegin()) throw runtime_error();
			int p = n;
			node *mo = it;
			if(it == base -> tail)
			{
				block *k = base -> block_head;
				while(k -> next != NULL)
					k = k -> next;
				node *m = k -> head;
				while(m -> next != NULL)
					m = m -> next;
				mo = m;
				p --;
			}
			while(p > 0 && mo -> pre != mo -> pos -> head)
			{
				p --;
				mo = mo -> pre;
			}
			if(p == 0) return const_iterator(mo,base);
			block * lp = mo -> pos;
			lp = lp -> pre;
			while(lp != base -> block_head && p > lp -> len)
			{
				p -= lp -> len;
				lp = lp -> pre;
			}
			if(lp == base -> block_head) throw runtime_error();
			node * lo = lp -> head;
			while(lo -> next != NULL)
				lo = lo -> next;
			while(-- p)
				lo = lo -> pre;
			return const_iterator(lo, base);
		}
		int operator-(const const_iterator &rhs) const 
		{
			if(base != rhs.base) throw invalid_iterator();
			node *k1 = it, *k2 = rhs.it;
			if(k1 == k2) return 0;
			int ans = 0;
			block *t1 = k1 -> pos, *t2 = k2 -> pos;
			if(it == base -> tail)
			{
				ans = 1;
				block * blo_t = base -> block_head;
				while(blo_t -> next != NULL)
					blo_t = blo_t -> next;
				k1 = blo_t -> head;
				while (k1 -> next != NULL)
					k1 = k1 -> next;
				t1 = blo_t;
			}
			else if(rhs.it == rhs.base -> tail)
				return -(rhs - *this);
			
			while(t2 != NULL && t2 != t1)
				t2 = t2 -> next;
			if(t2 == NULL)
				return -(rhs - *this);

			if(k1 -> pos == k2 -> pos)
			{
				int p = 0;
				while(k2 != NULL && k2 != k1)
				{
					p ++;
					k2 = k2 -> next;
				}
				if(k2 == NULL) return -(rhs - *this);
				else return (p + ans);
			}
			else
			{
				int al = 0;
				t1 = k1 -> pos; t2 = k2 -> pos;
				while(t2 != NULL && t2 != t1)
				{
					t2 = t2 -> next;
					al += t2 -> len;
				}
				al -= t2 -> len;
				while(k2 != NULL)
				{
					k2 = k2 -> next;
					al ++;
				}
				al --;
				node *k3 = t1 -> head;
				while(k3 != k1)
				{
					k3 = k3 -> next;
					al ++;
				}
				return (al + ans);	
			}
		}
		const_iterator& operator+=(const int &n) 
		{
			*this = *this + n;
			return *this;
		}
		const_iterator& operator-=(const int &n) 
		{
			*this = *this - n;
			return *this;
		}
		const_iterator operator++(int) 
		{
			const_iterator tmp = *this;
			*this += 1;
			return tmp;
		}
		const_iterator& operator++() 
		{
			*this += 1;
			return *this;
		}
		const_iterator operator--(int) 
		{
			const_iterator tmp = *this;
			*this -= 1;
			return tmp;
		}
		const_iterator& operator--() 
		{
			*this -= 1;
			return *this;
		}
		T& operator*() const 
		{
			if(it == base -> tail) throw invalid_iterator();
			return (it -> data);
		}
		T* operator->() const noexcept 
		{
			if(it == base -> tail) throw invalid_iterator();
			return &(it -> data);
		}
		bool operator==(const iterator &rhs) const 
		{
			return (it == rhs.it);
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return (it == rhs.it);
		}
		bool operator!=(const iterator &rhs) const 
		{
			return (it != rhs.it);
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return (it != rhs.it);
		}
	};
	/**
	 * TODO Constructors
	 */
	deque() 
	{
		block_head = new block(NULL, NULL, NULL, 0);
		tail = (node *)(operator new(sizeof(node)));
		num_of_block = num_of_data = 0;
		maxlen = 512;
	}
	deque(const deque &other) 
	{
		if(other.num_of_data == 0)
		{
			block_head = new block(NULL, NULL, NULL, 0);
			tail = (node *)(operator new(sizeof(node)));
			num_of_block = num_of_data = 0;
			maxlen = 512;
		}
		else
		{
			num_of_data = other.num_of_data;
			maxlen = other.maxlen;
			num_of_block = other.num_of_block;
			tail = (node *)operator new(sizeof(node));
			block_head = new block;
			block *my = block_head, *myp = my, *ot = other.block_head;
			while(ot -> next != NULL)
			{
				myp = my;
				ot = ot -> next;
				my -> next = new block(NULL, NULL, myp, ot -> len);
				my = my -> next;
			
				node *np = NULL, *on = ot -> head;
				my -> head = (node*)operator new(sizeof(node));
				node *n = my -> head;
				n -> pre = NULL; n -> pos = my; n -> next = NULL;

				for(int j = 1;j <= my -> len;++ j)
				{
					on = on -> next;
					np = n;
					n -> next = new node(on -> data, NULL, np, my);
					n = n -> next;
				}
			}
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() 
	{
		clear();
		delete block_head;
		operator delete((void*)tail,sizeof(node));
	}

	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) 
	{
        if(this == &other) return *this;
		if(other.num_of_data == 0)
		{
			clear();
			block_head = new block(NULL, NULL, NULL, 0);
			tail = (node *)(operator new(sizeof(node)));
			num_of_block = num_of_data = 0;
			maxlen = 512;
			return *this;
		}
		clear();
		num_of_data = other.num_of_data;
		maxlen = other.maxlen;
		num_of_block = other.num_of_block;

		tail = (node *)operator new(sizeof(node));
		block_head = new block;
		block *my = block_head, *myp = my, *ot = other.block_head;

		while(ot -> next != NULL)
		{
			myp = my;
			ot = ot -> next;
			my -> next = new block(NULL, NULL, myp, ot -> len);
			my = my -> next;
			node *np = NULL, *on = ot -> head;
			my -> head = (node*)operator new(sizeof(node));
			node *n = my -> head;
			n -> pre = NULL; n -> pos = my; n -> next = NULL;
			for(int j = 1;j <= my -> len;++ j)
			{
				on = on -> next;
				np = n;
				n -> next = new node(on -> data, NULL, np, my);
				n = n -> next;
			}
		}
		return *this;
	}

	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() 
	{
		if(num_of_data == 0) return iterator(tail, this);
		block * t = block_head -> next;
		return iterator(t -> head -> next, this);
	}
	const_iterator cbegin() const 
	{
		if(num_of_data == 0) return const_iterator(tail, this);
		block * t = block_head -> next;
		return const_iterator(t -> head -> next, this);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() 
	{
		return iterator(tail, this);
	}
	const_iterator cend() const 
	{
		return const_iterator(tail, this);
	}

	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) 
	{
		if(pos >= num_of_data) throw index_out_of_bound();
		iterator tmp = begin() + pos;
		return (tmp.it) -> data;
	}
	const T & at(const size_t &pos) const 
	{
		if(pos >= num_of_data) throw index_out_of_bound();
		const_iterator tmp = cbegin() + pos;
		return (tmp.it) -> data;
	}
	T & operator[](const size_t &pos)
	{
		if(pos >= num_of_data) throw index_out_of_bound();
		iterator tmp = begin() + pos;
		return (tmp.it) -> data;
	}
	const T & operator[](const size_t &pos) const 
	{
		if(pos >= num_of_data) throw index_out_of_bound();
		const_iterator tmp = cbegin() + pos;
		return (tmp.it) -> data;
	}

	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const 
	{
		if(num_of_data == 0) throw container_is_empty();
		block *tmp = block_head -> next;
		node * tmp2 = tmp -> head -> next;
		return tmp2 -> data;
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const 
	{
		if(num_of_data == 0) throw container_is_empty();
		
		block * tmp = block_head;
		while(tmp -> next != NULL)
			tmp = tmp -> next;
		node * k = tmp -> head;
		while(k -> next != NULL)
			k = k -> next;
		return k -> data;
	}

	/**
	 * checks whether the container is empty.
	 */
	bool empty() const 
	{
		return (num_of_data == 0);
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const 
	{
		return num_of_data;
	}
	/**
	 * clears the contents
	 */
	void clear() //tail和block_head不动
	{
		if(num_of_data == 0) return;
		block * tmp1 = block_head -> next,*tmp2;
		while (tmp1 != NULL)
		{
			tmp2 = tmp1 -> next;
			if(tmp1 -> head == NULL) 
			{
				tmp1 = tmp2;
				continue;
			}
			node *k1 = tmp1 -> head -> next, *k2;
			while(k1 != NULL)
			{
				k2 = k1 -> next;
				delete k1;
				k1 = k2;
			}
			operator delete((void*)tmp1 -> head,sizeof(node));
			delete tmp1;
			tmp1 = tmp2;
		}
		num_of_block = num_of_data = 0;
		maxlen = 512;
		return;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) 
	{
		if(pos.base != this) throw runtime_error();
		node *p = pos.it;
		deque* bas = pos.base;
		if(p == bas -> tail)
		{
			if(bas -> num_of_data == 0) 
			{	
				bas -> beg(value);
				return iterator(bas -> block_head -> next -> head -> next, bas);
			}
			block *pn = bas -> block_head;
			while(pn -> next != NULL)
				pn = pn -> next;
			node *pp = pn -> head;
			while(pp -> next != NULL)
				pp = pp -> next;
			node * ins = new node(value, NULL, pp, pn);
			pp -> next = ins;
			bas -> num_of_data ++;
			pn -> len ++;
			if(pn -> len > maxlen) split(pn);
			//bas -> balance();
			return iterator(ins, bas);
		}
		
		block* pn = p -> pos;
		
		node *pp = p -> pre;
		pn -> len ++;
		bas -> num_of_data ++;
		node *n = new node(value, p, pp, pn);
		p -> pre = n;
		if(pp != NULL)
			pp -> next = n;
		if(pn -> len > maxlen) split(pn);
		//bas -> balance();
		return iterator(n, bas);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) 
	{
		if(pos.base != this) throw runtime_error();
		node *p = pos.it;
		deque* bas = pos.base;
		if(p == bas -> tail) throw invalid_iterator();
		if(bas -> num_of_data == 0) throw container_is_empty();
		block* pn = p -> pos;
		node *ne = p -> next, *pr = p -> pre;
		if(pr != NULL)
			pr -> next = ne;
		if(ne != NULL)
			ne -> pre = pr;
		delete p;
		pn -> len --;
		bas -> num_of_data --;
		if(pn -> len == 0)
		{
			block *ppn = pn -> next, *ppr = pn -> pre;
			if(ppr != NULL)
				ppr -> next = ppn;
			if(ppn != NULL)
			{
				ppn -> pre = ppr;
				operator delete((void *)pn -> head, sizeof(node));
				delete pn;
				bas -> num_of_block --;
				return iterator(ppn -> head -> next, bas);
			}
			else
			{
				operator delete((void*)pn -> head,sizeof(node));
				delete pn;
				bas -> num_of_block --;
				return iterator(bas -> tail, bas);
			}
		}
		else
		{
			if(ne != NULL)
			{
				iterator ret(ne, bas);
				if(pn -> next != NULL && pn -> len + pn -> next -> len < maxlen)
					merge(pn, pn -> next);
				//balance();
				return ret;
			}
			else
			{
				if(pn -> next != NULL)
				{
					iterator ret(pn -> next -> head -> next, bas);
					if(pn -> len + pn -> next -> len < maxlen)
						merge(pn, pn -> next);
					//balance();
					return ret;
				}
				else
					return iterator(bas -> tail, bas);
			}
		}
	}
	/*iterator erase(iterator pos) 
	{
		if(pos.base != this) throw runtime_error();
		node *p = pos.it;
		deque* bas = pos.base;
		if(p == bas -> tail) throw invalid_iterator();
		if(bas -> num_of_data == 0) throw container_is_empty();
		block* pn = p -> pos;
		node *ne = p -> next, *pr = p -> pre;
		if(pr != NULL)
			pr -> next = ne;
		if(ne != NULL)
			ne -> pre = pr;
		delete p;
		pn -> len --;
		bas -> num_of_data --;
		if(ne != NULL)
		{
			bas -> balance();
			return iterator(ne, bas);
		}
		else
		{
			if(pn -> next != NULL)
			{
				pn = pn -> next;
				ne = pn -> head -> next;
				bas -> balance();
				return iterator(ne, bas);
			}
			else
			{
				bas -> balance();
				return iterator(bas -> tail, bas);
			}
		}
	}*/
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) 
	{
		if(num_of_data == 0)
		{
			beg(value);
			return;
		}
		block * tmp1 = block_head;
		while(tmp1 -> next != NULL)
			tmp1 = tmp1 -> next;
		tmp1 -> len ++;
		node * tmp2 = tmp1 -> head;
		while(tmp2 -> next != NULL)
			tmp2 = tmp2 -> next;
		node * tmp3 = tmp2;
		tmp2 -> next = new node(value, NULL, tmp3, tmp1);
		num_of_data ++;
		if(tmp1 -> len > maxlen) split(tmp1);
		//balance();
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() 
    {
		if(num_of_data == 0) throw container_is_empty();
		if(num_of_data == 1) { clear(); return; }
		block * tmp1 = block_head, *tmp4 = block_head;
		while(tmp1 -> next != NULL)
		{
			tmp4 = tmp1;
			tmp1 = tmp1 -> next;
		}
		node * tmp2 = tmp1 -> head, *tmp3 = tmp1 -> head;
		if(tmp1 -> len == 1)
		{
			tmp2 = tmp2 -> next;
			tmp4 -> next = NULL;
			delete tmp2;
			operator delete((void*)tmp3,sizeof(node));
			delete tmp1;
			num_of_data --;
			num_of_block --;

		}
		else
		{
			while(tmp2 -> next != NULL)
			{
				tmp3 = tmp2;
				tmp2 = tmp2 -> next;
			}
			tmp1 -> len --;
			tmp3 -> next = NULL;
			delete tmp2;
			num_of_data --;
			//balance();
		}
    }
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) 
    {
		if(num_of_data == 0)
		{
			beg(value);
			return;
		}
		block * tmp1 = block_head -> next;
		node * tmp2 = tmp1 -> head -> next;
		tmp1 -> head -> next = new node(value, tmp2, tmp1 -> head, tmp1);
		tmp2 -> pre = tmp1 -> head -> next;
		tmp1 -> len ++;
		num_of_data ++;
		if(tmp1 -> len > maxlen) split(tmp1);
		//balance();
    }
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() 
	{
		if(num_of_data == 0) throw container_is_empty();
		if(num_of_data == 1) 
		{
			clear();
			return;
		}
		block * tmp1 = block_head -> next;
		if(tmp1 -> len == 1)
		{
			node *tmp2 = tmp1 -> head, *tmp3 = tmp2 -> next;
			block * ad = tmp1 -> next;
			block_head -> next = ad;
			if(ad != NULL)
				ad -> pre = block_head;
			delete tmp3;
			operator delete((void*)tmp2,sizeof(node));
			delete tmp1;
			num_of_data --;
			num_of_block --;
		}
		else
		{
			node * tmp2 = tmp1 -> head -> next;
			node * tmp3 = tmp2 -> next;
			tmp1 -> len --;
			num_of_data --;
			tmp1 -> head -> next = tmp3;
			if(tmp3 != NULL)
				tmp3 -> pre = tmp1 -> head;
			delete tmp2;
			if(tmp1 -> next != NULL && tmp1 -> len + tmp1 -> next -> len < maxlen)
				merge(tmp1, tmp1 -> next);
		}
	}
};

}

#endif
