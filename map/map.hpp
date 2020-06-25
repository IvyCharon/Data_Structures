/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	typedef pair<const Key, T> value_type;
	struct node
	{
		value_type data;
		node *left = NULL, *right = NULL, *parent = NULL;
		node(const value_type &d, node *l = NULL, node *r = NULL, node *p = NULL) : data(d),left(l),right(r),parent(p) {}
	};
	node *root, *tail;
	int num_of_data;
	Compare comp;
	node *ask(const Key &key) const 
	{
		if(num_of_data == 0) return NULL;
		node *tmp = root;
		while(tmp != NULL)
		{
			if (comp((tmp -> data).first, key)) tmp = tmp -> right;
			else if (comp(key, (tmp -> data).first)) tmp = tmp -> left;
			else break;
		}
		return tmp;
	}
	void makeEmpty(node * &t)
	{
		if(t == NULL) return;
		if(t -> left != NULL) makeEmpty(t -> left);
		if(t -> right != NULL) makeEmpty(t -> right);
		delete t;
	}
	void copy(node * &my, node * others, node * &mypa)
	{
		if(my != NULL && my != root) delete my;
		if(mypa == tail)
		{
			my = new node(others -> data, NULL, NULL, NULL);
			root = my;
		}
		else
		{
			my = new node(others -> data, NULL, NULL, mypa);
			if(comp((my -> data).first, (mypa -> data).first)) mypa -> left = my;
			else if(comp((mypa -> data).first,(my -> data).first)) mypa -> right = my;
		}
		if(others -> left != NULL) copy(my -> left, others -> left, my);
		if(others -> right != NULL) copy(my -> right, others -> right, my);
	}
	node * find_next(node * t) const
	{
		if (t -> right != NULL)
        {
			t = t->right;
			while(t -> left != NULL)
				t = t -> left;
			return t;
		}
		node * tmp = t -> parent;
		while(tmp != NULL)
        {
			if (tmp -> left == t) return tmp;
			t = tmp;
			tmp = t -> parent;
		}
		return tail;
	}
	node * find_front(node * t) const
	{
		if(t == tail)
		{
			t = root;
			if(t == NULL) throw runtime_error();
			while(t -> right != NULL)
				t = t -> right;
			return t;
		}
		if(t -> left != NULL)
		{
			t = t -> left;
			while(t -> right != NULL)
				t = t -> right;
			return t;
		}
		node * tmp = t -> parent;
		while(tmp != NULL)
		{
			if (tmp -> right == t) return tmp;
			t = tmp;
			tmp = t -> parent;
		}
		throw invalid_iterator();
	}
	
	class const_iterator;
	class iterator 
	{
		friend class map;
		friend class const_iterator;
	private:
		const map *base;
		node *it;
	public:
		iterator() : base(NULL), it(NULL) {}
		iterator(node *tmp,const map *mp) : base(mp), it(tmp) {}
		iterator(const iterator &other) 
		{
			base = other.base;
			it = other.it;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
			if(it == base -> tail) throw invalid_iterator();
			iterator tmp(it,base);
			it = base -> find_next(it);
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++()
		{
			if(it == base -> tail) throw invalid_iterator();
			it = base -> find_next(it);
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			if(base -> root == NULL) throw invalid_iterator();
			if(it == base -> tail)
			{
				node * tmp = base -> root;
				while(tmp -> right != NULL)
					tmp = tmp -> right;
				iterator tp(it,base);
				it = tmp;
				return tp;
			}
			node * tmp = base -> root;
			while(tmp -> left != NULL)
				tmp = tmp -> left;
			if(it == tmp) throw invalid_iterator();
			iterator tp(it,base);
			it = base -> find_front(it);
			return tp;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() 
		{
			if(base -> root == NULL) throw invalid_iterator();
			if(it == base -> tail)
			{
				node * tmp = base -> root;
				while(tmp -> right != NULL)
					tmp = tmp -> right;
				it = tmp;
				return *this;
			}
			node * tmp = base -> root;
			while(tmp -> left != NULL)
				tmp = tmp -> left;
			if(it == tmp) throw invalid_iterator();
			it = base -> find_front(it);
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const 
		{
			if(it == NULL) throw invalid_iterator();
			return it -> data;
		}
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

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept 
		{
			if(it == NULL) throw invalid_iterator();
			return &(it -> data);
		}
	};
	class const_iterator 
	{
		friend class map;
		friend class iterator;
	private:
		const map *base;
		node *it;
	public:
		const_iterator() : base(NULL), it(NULL)	{}
		const_iterator(node *tmp,const map *mp) : base(mp), it(tmp) {}
		const_iterator(const const_iterator &other) 
		{
			base = other.base;
			it = other.it;
		}
		const_iterator(const iterator &other) 
		{
			base = other.base;
			it = other.it;
		}

		const_iterator operator++(int) 
		{
			if(it == base -> tail) throw invalid_iterator();
			const_iterator tmp(it,base);
			it = base -> find_next(it);
			return tmp;
		}
		const_iterator & operator++() 
		{
			if(it == base -> tail) throw invalid_iterator();
			it = base -> find_next(it);
			return *this;
		}

		const_iterator operator--(int) 
		{
			if(base -> root == NULL) throw invalid_iterator();
			if(it == base -> tail)
			{
				node * tmp = base -> root;
				while(tmp -> right != NULL)
					tmp = tmp -> right;
				const_iterator tp(it,base);
				it = tmp;
				return tp;
			}
			node * tmp = base -> root;
			while(tmp -> left != NULL)
				tmp = tmp -> left;
			if(it == tmp) throw invalid_iterator();
			const_iterator tp(it,base);
			it = base -> find_front(it);
			return tp;
		}
		const_iterator & operator--() 
		{
			if(base -> root == NULL) throw invalid_iterator();
			if(it == base -> tail)
			{
				node * tmp = base -> root;
				while(tmp -> right != NULL)
					tmp = tmp -> right;
				it = tmp;
				return *this;
			}
			node * tmp = base -> root;
			while(tmp -> left != NULL)
				tmp = tmp -> left;
			if(it == tmp) throw invalid_iterator();
			it = base -> find_front(it);
			return *this;
		}

		value_type & operator*() const 
		{
			if(it == NULL) throw invalid_iterator();
			return it -> data;
		}
		bool operator==(const const_iterator &rhs) const 
		{
			return (it == rhs.it);
		}
		bool operator==(const iterator &rhs) const 
		{
			return (it == rhs.it);
		}

		bool operator!=(const const_iterator &rhs) const 
		{
			return (it != rhs.it);
		}
		bool operator!=(const iterator &rhs) const 
		{
			return (it != rhs.it);
		}

		value_type* operator->() const noexcept 
		{
			if(it == NULL) throw invalid_iterator();
			return &(it -> data);
		}
	};
	/**
	 * TODO two constructors
	 */
	map() 
	{
		num_of_data = 0;
		root = NULL;
		tail = (node *)(operator new(sizeof(node)));
	}
	map(const map &other) 
	{
		root = NULL;
		num_of_data = other.num_of_data;
		tail = (node *)(operator new(sizeof(node)));
		if (other.root == NULL) root = NULL;
		else
			copy(root,other.root,tail);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) 
	{
		if(this == &other) return *this;
		clear();
		root = NULL;
		num_of_data = other.num_of_data;
		if(tail != NULL)
			operator delete(tail);
		tail = (node *)(operator new(sizeof(node)));
		if (other.root == NULL) root = NULL;
		else
			copy(root,other.root,tail);
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() 
	{
		clear();
		operator delete(tail);
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) 
	{
		node * tmp = ask(key);
		if(tmp == NULL) throw index_out_of_bound();
		return (tmp -> data).second;
	}
	const T & at(const Key &key) const 
	{
		node * tmp = ask(key);
		if(tmp == NULL) throw index_out_of_bound();
		return (tmp -> data).second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) 
	{
		node * tmp = ask(key);
		if(tmp == NULL)
		{
			pair<iterator, bool> mm = insert(value_type(key, T()));
			iterator kk = mm.first;
			node * ans = kk.it;
			return (ans -> data).second;
		}
		else return (tmp -> data).second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const 
	{
		node * tmp = ask(key);
		if(tmp == NULL) throw index_out_of_bound();
		return (tmp -> data).second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() 
	{
		if(root == NULL) return iterator(tail,this);
		node * tmp = root;
		while(tmp -> left != NULL)
			tmp = tmp -> left;
		return iterator(tmp, this);
	}
	const_iterator cbegin() const 
	{
		if(root == NULL) return const_iterator(tail,this);
		node * tmp = root;
		while(tmp -> left != NULL)
			tmp = tmp -> left;
		return const_iterator(tmp, this);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
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
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const 
	{
		return (num_of_data == 0);
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const 
	{
		return num_of_data;
	}
	/**
	 * clears the contents
	 */
	void clear() 
	{
		if(num_of_data != 0)
			makeEmpty(root);
		num_of_data = 0;
		root = NULL;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) 
	{
		if(num_of_data == 0)
		{
			root = new node(value, NULL, NULL, NULL);
			num_of_data ++;
			return pair<iterator, bool>(iterator(root,this),true);
		}
		node * tmp = root;
		node * pa = NULL;
		while(tmp != NULL)
		{
			pa = tmp;
			if(comp(value.first, (tmp -> data).first)) 
				tmp = tmp -> left;
			else if(comp((tmp -> data).first, value.first))
				tmp = tmp -> right;
			else break;
		}
		if(tmp != NULL)
			return pair<iterator, bool>(iterator(tmp, this), false);
		else
		{
			node *t = new node(value,NULL,NULL,pa);
			num_of_data ++;
			if (pa != NULL)
			{
				if (comp(value.first, (pa->data).first)) pa->left = t;
				else if(comp((pa -> data).first, value.first)) pa->right = t;
			}
			return pair<iterator, bool>(iterator(t, this),true);
		}		
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) 
	{
		if(pos.base != this || pos == this -> end() || pos.it == NULL) throw runtime_error();
		node * t = pos.it;
		if(t -> left == NULL && t -> right != NULL)
		{
			if(t -> parent == NULL) root = t -> right;
			else if(t == t -> parent -> left) t -> parent -> left = t -> right;
			else t -> parent -> right = t -> right;
			t -> right -> parent = t -> parent;
		}
		else if(t -> right == NULL && t -> left != NULL)
		{
			if(t -> parent == NULL) root = t -> left;
			else if(t == t -> parent -> left) t -> parent -> left = t -> left;
			else t -> parent -> right = t -> left;
			t -> left -> parent = t -> parent;
		}
		else if(t -> right == NULL && t -> left == NULL)
		{
			if(t -> parent != NULL)
			{
				if(t -> parent -> left == t) t -> parent -> left = NULL;
				else t -> parent -> right = NULL;
			}
			else
				root = NULL;
		}
		else
		{
			node * tmp = t -> right;
			while(tmp -> left != NULL)
				tmp = tmp -> left;
			tmp -> left = t -> left;
			t -> left -> parent = tmp;
			if(tmp -> parent != t)
			{
				node * tmp_pa = tmp -> parent;
				if(tmp -> right != NULL)
				{
					if(tmp == tmp_pa -> left) tmp_pa -> left = tmp -> right;
					else tmp_pa -> right = tmp -> right;
					tmp -> right -> parent = tmp_pa;
				}
				else
				{
					if(tmp == tmp_pa -> left) tmp_pa -> left = NULL;
					else tmp_pa -> right = NULL; 
				}
				tmp -> right = t -> right;
				t -> right -> parent = tmp;
			}
			tmp -> parent = t -> parent;
			if(t -> parent == NULL)
				root = tmp;
			else
			{
				if(t == t -> parent -> left) t -> parent -> left = tmp;
				else t -> parent -> right = tmp;
			}
		}
		delete t;
		num_of_data --;
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const 
	{
		const node * tmp = ask(key);
		return (tmp != NULL);
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) 
	{
		node * tmp = ask(key);
		if(tmp == NULL) return end();
		else return iterator(tmp, this);
	}
	const_iterator find(const Key &key) const 
	{
		node * tmp = ask(key);
		if(tmp == NULL) return cend();
		else return const_iterator(tmp, this);
	}
};

}

#endif
