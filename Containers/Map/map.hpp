#ifndef MAP_HPP
# define MAP_HPP

# include <iostream>
# include <memory>

# include <algorithm>
# include "mapNode.hpp"
# include "../../Additionals/BiDirectionalIterator.hpp"
# include "../../Additionals/Traits.hpp"
# include "pair.hpp"
# include <map>
# include <string>

#define CRESET   "\033[0m"
#define CRED     "\033[31m"      /* Red */
#define CGREEN   "\033[32m"      /* Green */
#define CYELLOW  "\033[33m"      /* Yellow */
#define CBLUE    "\033[34m"      /* Blue */

namespace ft
{
template <class Key, class T, class Compare = std::less<Key>, class Alloc = std::allocator<std::pair<const Key,T> > > 
class map{

  public:
	typedef Key                                                 key_type;
	typedef T                                                   mapped_type;
	typedef ft::pair<Key, T>                              		value_type;
	typedef Compare                                             key_compare;
	typedef Alloc                                               allocator_type;
	typedef value_type&                                         reference;
	typedef const value_type&                                   const_reference;
	typedef value_type*                                         pointer;
	typedef const value_type*                                   const_pointer;
	typedef std::ptrdiff_t                                      difference_type;
	typedef size_t                                              size_type;

	typedef mapNode<value_type>*                                node_pointer;
	typedef mapNode<value_type>                                 node;

	typedef BiDirectionalIterator<value_type, node> 			iterator;
	typedef ConstBiDirectionalIterator<value_type, node> 		const_iterator;
	typedef ReverseBiDirectionalIterator<value_type, node> 		reverse_iterator;
	typedef ConstReverseBiDirectionalIterator<value_type, node> const_reverse_iterator;
  private: 
	node_pointer 	_root;
	node_pointer 	_first;
	node_pointer 	_last;
	size_type		_size;
	key_compare		_compare;
	allocator_type 	_alloc;

  public:

   class value_compare
    {
        friend class map;
      protected:
        Compare _compare;
        value_compare (Compare compare) : _compare(compare) {}
      public:
        bool operator() (const value_type& x, const value_type& y) const {
            return _compare(x.first, y.first);}
    };

	//      CONSTRUCTORS
	explicit map(const key_compare& compare = key_compare(), const allocator_type& alloc = allocator_type())
	{
		_size = 0;
		_compare = compare;
		_alloc = alloc;
		make_tree();
	}

	template <class InputIterator>
    map(InputIterator first, InputIterator last, const key_compare& compare = key_compare(),
        const allocator_type& alloc = allocator_type())
	{
		_size = 0;
		_compare = compare;
		_alloc = alloc;
		make_tree();
        insert(first, last);
    }

	map(const map& other){
		_size = 0;
		_compare = other._compare;
		_alloc = other._alloc;
		make_tree();
        insert(other.begin(), other.end());
	};

	~map(){
		clear();
		delete _root;
		delete _first;
		delete _last;
	}

	map 				operator=(const map& other)
	{
		clear();
		_compare = other._compare;
		_alloc = other._alloc;
        insert(other.begin(), other.end());
		return *this;
	}


	// 		ITERATORS
	iterator						begin(){ return iterator(_first->parent);};	
	const_iterator					begin() const { return const_iterator(_first->parent);};	
	iterator						end(){ return iterator(_last);};	
	const_iterator					end() const { return const_iterator(_last);};	
	reverse_iterator				rbegin() { return reverse_iterator(_last->parent);};	
	const_reverse_iterator			rbegin() const { return const_reverse_iterator(_last->parent);};	
	reverse_iterator				rend(){ return reverse_iterator(_first);};	
	const_reverse_iterator			rend() const { return const_reverse_iterator(_first);};	

	bool							empty() const {return _size == 0;}
	size_type						size() const {return _size;}
	size_type 						max_size() const{ return _alloc.max_size();}

	mapped_type& 					operator[] (const key_type& k){
		iterator it = find(k);
		if (it == end()){
			value_type pair(k, mapped_type());
			insert(pair);
			return find(k)->second;
		}
		return it->second;
	};

	pair<iterator, bool>        insert(const value_type& val) // verander de return hier nog naar pair....
	{
		bool ret = false;
		if (_size == 0 || find(val.first) == end())
			ret = true;
		if (_size == 0){
			this->_root->data = val;
			_size = 1;
			_first->parent = _root;
			_last->parent = _root;
		}
		else {
			node_pointer tmp = _root;
			searchSpot(val, tmp);
		}
		RBT_Rules();
		iterator it = find(val.first);
		return ft::pair<iterator, bool>(it, ret); // check dit even
	}

	iterator 					insert(iterator position, const value_type& val){
		(void)position;
		return insert(val).first;
	}

	template <class InputIterator>
    void insert (typename enable_if<is_input_iterator<InputIterator>::value, InputIterator>::type first, InputIterator last)
	{
        for (;first != last; ++first) {
            insert(*first);
        }
    }


	size_type 				erase(const key_type& k)
	{
		iterator it = find(k);
		if (it == end())
			return 0;
		erase(it);
		return (1);
	}

	void 					erase(iterator first, iterator last)
	{
		iterator tmp = first;
		while (first != last){
			tmp++;
			erase(first);
			first = tmp;
		} 
	}

	void 					erase(typename enable_if<is_input_iterator<iterator>::value, iterator>::type position)
	{
		node_pointer old = position.getNode();
		if (!old || old == _last || old == _first)
			return ;
		if (_size == 1)
			return deleteLast();
		node_pointer replace = NULL;
		node_pointer p = NULL;
		node_pointer x = NULL;
		node_pointer w = NULL;
		bool x_is_left = false;
		bool old_col = old->color; // save color of [to be erased]
		bool replace_col = BLACK;

		x = step1(old, &replace);
		w = step2(replace, old, &p, &replace_col); // save color of [replacing node]
		if (splice(replace, x, old))
			x_is_left = true;
		replace_func(old, replace);
		if (old_col == RED && (replace_col == RED || !replace)){
			if (replace)
				replace->color = RED;
		}
		else if (old_col == BLACK && replace_col == RED){
			replace->color = BLACK;
		}
		else {
			if (old_col == RED && replace_col == BLACK && replace){
				replace->color = RED;
				step3(x, w, p, x_is_left, 1);
			}
			else 
				step3(x, w, p, x_is_left, 0);
		}
	}

	void 					swap(map& other){
		node_pointer 	tmp_root  = other._root;
		node_pointer 	tmp_first = other._first;
		node_pointer 	tmp_last = other._last;
		size_type		tmp_size = other._size;
		key_compare		tmp_compare = other._compare;
		allocator_type 	tmp_alloc = other._alloc;

		other._root = this->_root;
		other._first = this->_first;
		other._last = this->_last;
		other._alloc = this->_alloc;
		other._compare = this->_compare;
		other._size = this->_size;

		this->_root = tmp_root;
		this->_first = tmp_first;
		this->_last = tmp_last; 
		this->_alloc = tmp_alloc;
		this->_compare = tmp_compare;
		this->_size = tmp_size;
	}

	void					clear()
	{
		deleteAll(_root);
		_size = 0;
		make_tree();
	}

	//OBSERVERS
	key_compare				key_comp() {return _compare;};
	value_compare			value_comp() {return value_compare(_compare);};

	iterator						find(const key_type& k){
		node_pointer tmp = _root;
		while (tmp != _first && tmp != _last){
			if (k == tmp->data.first)
				return iterator(tmp);
			if (_compare(k, tmp->data.first)){
				if (tmp->left)
					tmp = tmp->left;
				else 
					return end();
			}
			else {
				if (tmp->right)
					tmp = tmp->right;
				else 
					return end();
			}
		} 
		return end();
	}

	const_iterator				find(const key_type& k) const{
		node_pointer tmp = _root;
		while (tmp != _first && tmp != _last){
			if (k == tmp->data.first)
				return const_iterator(tmp);
			if (_compare(k, tmp->data.first)){
				if (tmp->left)
					tmp = tmp->left;
				else 
					return end();
			}
			else {
				if (tmp->right)
					tmp = tmp->right;
				else 
					return end();
			}
		} 
		return end();
	}

	size_type 					count(const key_type& k) const
	{
		if (find(k) != end())
			return 1;
		return 0;
	}

	iterator					lower_bound(const key_type& k)
	{
		for (iterator it = begin(); it != end(); ++it)
			if (!_compare(it->first, k))
				return it;
		return end();
	}

	const_iterator				lower_bound(const key_type& k) const
	{
		for (iterator it = begin(); it != end(); ++it)
			if (!_compare(it->first, k))
				return it;
		return end();
	}

	iterator					upper_bound(const key_type& k)
	{
		for (iterator it = begin(); it != end(); ++it)
			if (_compare(k, it->first))
				return it;
		return end();
	}

	const_iterator				upper_bound(const key_type& k) const
	{
		for (iterator it = begin(); it != end(); ++it)
			if (_compare(k, it->first))
				return it;
		return end();
	}

	ft::pair<const_iterator, const_iterator> 	equal_range(const key_type& k) const {	
		return ft::pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));}

	ft::pair<iterator,iterator>             	equal_range(const key_type& k) {
		return ft::pair<iterator, iterator>(lower_bound(k), upper_bound(k));}

  private:
	node_pointer					step1(node_pointer old, node_pointer* replace){
		if ((old->right == NULL || old->right == _last) && (old->left == NULL || old->left == _first)){
			*replace = NULL;
			return NULL;
		}
		if (old->right == NULL || old->right == _last){
			*replace = old->left;
			return old->left;
		}
		if (old->left == NULL || old->left == _first){
			*replace = old->right;
			return old->right;
		}
		*replace = old->right;
		while ((*replace)->left != NULL && (*replace)->left != _first)
			(*replace) = (*replace)->left;
		return (*replace)->right;
	}
	
	node_pointer			step2(node_pointer replace, node_pointer old, node_pointer *p, bool *replace_col)
	{
		if (replace != NULL){
			*p = replace; //setting the parent which will be needed in the case that x and sibling will result to null
			*replace_col = replace->color; //setting the color of replace only if it exists
			if (replace->parent && replace->parent->right == replace){
				if (replace == old->right)
					return old->left;
				else 
					return replace->parent->left;
			}
			else if (replace->parent && replace->parent->left == replace){
				if (replace == old->right)
					return old->left;
				else 
					return replace->parent->right; // making w the sibling of x
			}
		}
		else {
			*replace_col = BLACK;
			if (old->parent){
				if (old == old->parent->left)
					return old->parent->right;
				else if (old == old->parent->right)
					return old->parent->left;
			}
		}
		return NULL;
	}

	void					step3(node_pointer x, node_pointer w, node_pointer p, bool x_is_left, bool original_x_color)
	{
		bool w_left_col = BLACK;
		bool w_right_col = BLACK;
		bool w_col = BLACK;
		bool x_col = BLACK;

		if (x == _root)
			return ;
		if (x && x->color == RED)// getting all colors of x w and children
			x_col = RED;
		if (original_x_color)
			x_col = BLACK;
		if (w && w->color == RED)
			w_col = RED;
		if (w && w->left && w->left->color == RED)
			w_left_col = RED;
		if (w && w->right && w->right->color == RED)
			w_right_col = RED;

		
		if (x_col == RED) // if x = RED
			case_0(x);
		else if (x_col == BLACK)
		{
			if (w_col == RED) // if x = black and w = red
				case_1(x, w, p, x_is_left);
			else if (w_left_col == BLACK && w_right_col == BLACK)
				case_2(x, w, p, x_is_left);
			else if ((x_is_left && w_left_col == RED && w_right_col == BLACK) || (!x_is_left && w_right_col == RED && w_left_col == BLACK))
				case_3(x, w, x_is_left);
			else if ((x_is_left && w_right_col == RED) || (!x_is_left && w_left_col == RED))
				case_4(x, w, x_is_left);
		}
	}

	bool							splice(node_pointer replace, node_pointer x, node_pointer old) // maak dit weer een void
	{
		if (x == NULL && replace == NULL){
			if (old->parent && old->parent->left == old)
				return true;
			return false;
		}
		if (x != NULL)
			x->parent = replace->parent;
		if (replace == replace->parent->left)
		{
			replace->parent->left = x;
			return true;
		}
		else 
			replace->parent->right = x;
		return false;
	}

	void							replace_func(node_pointer old, node_pointer replace)
	{
		if (_root == old){
			_root = replace;
			if (replace != NULL)
				replace->parent = NULL;
		}
		else {
			if (old == old->parent->left)
				old->parent->left = replace;
			else
				old->parent->right = replace;
			if (replace)
				replace->parent = old->parent;		
		}
		if (old->right != NULL && replace && replace != old->right)
			old->right->parent = replace;
		if (old->left != NULL && replace && replace != old->left)
			old->left->parent = replace;
		if (replace && replace != old->left)
			replace->left = old->left;
		if (replace && old->right != replace)
			replace->right = old->right;
		set_first();
		set_last();
		delete old;
		--_size;
	}

	void							set_first(){
		node_pointer tmp = _root;

		while (tmp->left != NULL && tmp->left != _first)
			tmp = tmp->left;
		_first->parent = tmp;
		tmp->left = _first;
	}

	void							set_last(){
		node_pointer tmp = _root;

		while (tmp->right != NULL && tmp->right != _last)
			tmp = tmp->right;
		_last->parent = tmp;
		tmp->right = _last;
	}

	void					case_0(node_pointer x)
	{
		x->color = BLACK;
	}

	void					case_1(node_pointer x, node_pointer w, node_pointer p, bool x_is_left)
	{
		p = w->parent;
		w->color = BLACK;
		if (p)
			p->color = RED;
		if (x_is_left)
			LeftRotate(w, w->parent);
		else
			RightRotate(w, w->parent);
		if (x_is_left) {
			if (p)
				w = p->right;
		}
		else {
			if (p)
				w = p->left;
		}
		step3(x, w, p, x_is_left, 0);
	}

	void					case_2(node_pointer x, node_pointer w, node_pointer p, bool x_is_left)
	{
		if (w)
			w->color = RED;
		if (x && x->parent)
			x = x->parent;
		else if (w && w->parent)
			x = w->parent;
		else 
			x = p;
		if (x && x->parent){
			p = x->parent;
			if (x == x->parent->left){
				w = x->parent->right;
				x_is_left = true;
			}
			else {
				w = x->parent->left; 
				x_is_left = false;
			}
		}
		else 
			p = NULL;

		if (x && x->color == RED)
			x->color = BLACK;
		else 
			step3(x, w, p, x_is_left, 0);
	}

	void					case_3(node_pointer x, node_pointer w, bool x_is_left)
	{
		if (x_is_left && w && w->left)
			w->left->color = BLACK;
		else if (!x_is_left && w && w->right)
			w->right->color = BLACK;
		if (w)
			w->color = RED;
		if (x_is_left){
			RightRotate(w->left, w);
			w = w->parent;
		}
		else { 
			LeftRotate(w->right, w);
			w = w->parent;
		}
		case_4(x, w, x_is_left);
	}

	void					case_4(node_pointer x, node_pointer w, bool x_is_left)
	{
		if (x && x->parent){
			if (w)
				w->color = x->parent->color;
			if (x->parent->left == x)
				x_is_left = true;
			else 
				x_is_left = false;
		}
		else if (w && w->parent){
			w->color = w->parent->color;
			if (w->parent->left == w)
				x_is_left = false;
			else
				x_is_left = true;
		}
		if (x && x->parent)
			x->parent->color = BLACK;
		else if (w && w->parent)
			w->parent->color = BLACK;
		if (x_is_left){
			if (w && w->right)
				w->right->color = BLACK;
			LeftRotate(w, w->parent);
		}
		else {
			if (w && w->left)
				w->left->color = BLACK;
			RightRotate(w, w->parent);
		}
	}

	void					deleteLast()
	{
		delete _first;
		delete _root;
		delete _last;
		_size = 0;
		make_tree();
	}

	void					deleteAll(node_pointer tmp)
	{
		if (tmp->right)
			deleteAll(tmp->right);
		if (tmp->left)
			deleteAll(tmp->left);
		delete tmp;
	}

	void				print_node(std::string root_path)
	{
		node_pointer tmp = _root;

		std::cout << ".";
		for (int i = 0; root_path[i]; ++i){
			if (root_path[i] == 'L'){
				if (tmp->left == NULL)
					return ;
				tmp = tmp->left;
			}
			if (root_path[i] == 'R'){
				if (tmp->right == NULL)
					return ;
				tmp = tmp->right;
			}
		}
		if (tmp->data.first){
			if (tmp->color)
				std::cout << CRED << tmp->data.first << CRESET;
			else 
				std::cout << CYELLOW << tmp->data.first << CRESET;
		}
	}

//   public: //Comment this in to make the print_tree work.
	void				print_tree()
	{
		std::string root_path;
		int layer = 0;
		root_path = "";
		int starting_tabs = 16;
		int starting_gap = 16;
		while (layer < 5)
		{
			root_path.clear();
			int tmp_tabs = starting_tabs;
			int tmp_gap = starting_gap;
			for (int tmp_layer = layer; tmp_layer; --tmp_layer)
			{
				root_path.append("L");
				tmp_gap = tmp_gap / 2;
				tmp_tabs -= tmp_gap;
			}
			while (root_path.find('L') != std::string::npos){
				if (root_path.find('R') == std::string::npos)
					for (; tmp_tabs; --tmp_tabs)
						std::cout << "   ";
				else 
					for (int tmp_gap2 = tmp_gap * 2; tmp_gap2; --tmp_gap2)
						std::cout << "   ";
				print_node(root_path);
				size_t L_found = root_path.find_last_of('L');
				root_path[L_found] = 'R';
				++L_found;
				for (;L_found != root_path.size(); ++L_found){
					root_path[L_found] = 'L';
				}
			}
			if (root_path.find('R') == std::string::npos)
				for (; tmp_tabs; --tmp_tabs)
					std::cout << "   ";
			else 
				for (int tmp_gap2 = tmp_gap * 2; tmp_gap2; --tmp_gap2)
					std::cout << "   ";
			print_node(root_path);
			std::cout << std::endl << std::endl << std::endl;
			layer++;
		}
	}

	void					manipulate(iterator x)
	{
		node_pointer tmp = x.getNode();

		tmp->color = RED;
		tmp->left->color = BLACK;
		tmp->right->color = BLACK;
	}

  private:  // INSERTING AND ROTATING
	bool				findAuntColorFLip(node_pointer tmp)
	{
		node_pointer& GP = tmp->parent->parent;
		if (GP->left != NULL && tmp->parent == GP->left){
			if (GP->right && GP->right->color == RED)
			{
				GP->right->color = BLACK;
				GP->left->color = BLACK;
				GP->color = RED;
				return RED;
			}
			return BLACK;
		}
		if (GP->right != NULL && tmp->parent == GP->right){
			if (GP->left && GP->left->color == RED)
			{
				GP->right->color = BLACK;
				GP->left->color = BLACK;
				GP->color = RED;
				return RED;
			}
			return BLACK;
		}
		return BLACK;
	}

	void				adjecentRedNodes(node_pointer tmp, node_pointer *tmp_check){
		if (tmp == _last)
			return ;
		if (tmp->color == RED){
			if (tmp->left != NULL && tmp->left->color == RED){
				(*tmp_check) = tmp->left;
				return ;
			}
			if (tmp->right != NULL && tmp->right->color == RED){
				(*tmp_check) = tmp->right;
				return ;
			}
		}
		if (tmp->left != NULL && tmp->left != _first)
			adjecentRedNodes(tmp->left, tmp_check);
		if (tmp->right != NULL)
			return adjecentRedNodes(tmp->right, tmp_check);
	}

	// ROTATERS
	void				RightRotate(node_pointer parent, node_pointer GP)
	{
		if (GP->parent != NULL){
			if (GP == GP->parent->right)
				GP->parent->right = parent;
			else
				GP->parent->left = parent;
		}
		if (parent->right != NULL){
			GP->left = parent->right;
			parent->right->parent = GP;
		}
		else 
		{
			GP->left = NULL;
		}
		parent->parent = GP->parent;
		GP->parent = parent;
		parent->right = GP;
		if (_root == GP)
			_root = parent;
	}

	void				LeftRotate(node_pointer parent, node_pointer GP)
	{
		if (GP->parent != NULL){
			if (GP == GP->parent->left)
				GP->parent->left = parent;
			else
				GP->parent->right = parent;
		}
		if (parent->left != NULL){
			GP->right = parent->left;
			parent->left->parent = GP;
		}
		else 
		{
			GP->right = NULL;
		}
		parent->parent = GP->parent;
		GP->parent = parent;
		parent->left = GP;
		if (_root == GP)
			_root = parent;
	}

	void				LeftRightRotate(node_pointer child)
	{
		node_pointer parent = child->parent;
		node_pointer GP = parent->parent;

		if (GP->parent != NULL){
			if (GP == GP->parent->right)
				GP->parent->right = child;
			else
				GP->parent->left = child;
			child->parent = GP->parent;
		}
		if (_root == GP)
			_root = child;
		if (child->right){
			child->right->parent = GP;
			GP->left = child->right;
		}
		else 
			GP->left = NULL;
		if (child->left){
			child->left->parent = parent;
			parent->right = child->left;
		}
		else 
			parent->right = NULL;

		child->right = GP;
		GP->parent = child;
		child->left = parent;
		parent->parent = child;

		parent->color = RED;
		GP->color = RED;
		child->color = BLACK;

	}

	void				RightLeftRotate(node_pointer child)
	{
		node_pointer parent = child->parent;
		node_pointer GP = parent->parent;

		if (GP->parent != NULL){
			if (GP == GP->parent->left)
				GP->parent->left = child;
			else
				GP->parent->right = child;
			child->parent = GP->parent;

		}
		if (_root == GP)
			_root = child;
		if (child->left){
			child->left->parent = GP;
			GP->right = child->left;
		}
		else 
			GP->right = NULL;
		if (child->right){
			child->right->parent = parent;
			parent->left = child->right;
		}
		else 
			parent->left = NULL;

		child->left = GP;
		GP->parent = child;
		child->right = parent;
		parent->parent = child;

		parent->color = RED;
		GP->color = RED;
		child->color = BLACK;
	}

	void				reColor(node_pointer child, node_pointer parent, node_pointer GP)
	{
		parent->color = BLACK;
		GP->color = RED;
		child->color = RED;
	}

	void				RBT_Rules(){
		node_pointer tmp = _root;
		node_pointer tmp_check = nullptr;
		if (_root->color == RED)
			_root->color = BLACK;
		adjecentRedNodes(tmp, &tmp_check);
		if (tmp_check != nullptr){
			if (findAuntColorFLip(tmp_check) == BLACK){
				if (tmp_check->parent->left != NULL && tmp_check == tmp_check->parent->left){
					if (tmp_check->parent->parent->left != NULL && tmp_check->parent == tmp_check->parent->parent->left){
						RightRotate(tmp_check->parent, tmp_check->parent->parent);
						reColor(tmp_check, tmp_check->parent, tmp_check->parent->right);
					}
					else 
						RightLeftRotate(tmp_check);
				}
				else { 
					if (tmp_check->parent->parent->right != NULL && tmp_check->parent == tmp_check->parent->parent->right){
						LeftRotate(tmp_check->parent, tmp_check->parent->parent);
						reColor(tmp_check, tmp_check->parent, tmp_check->parent->left);
					}
					else
						LeftRightRotate(tmp_check);
				}
			}
			RBT_Rules();
		}
	}		


	void                make_first(const value_type& val, node_pointer tmp)
	{
		node_pointer new_node = new node(val);

		tmp->left = new_node;
		new_node->parent = tmp;
		new_node->left = this->_first;
		this->_first->parent = new_node;
		++_size;

	}

	void                make_last(const value_type& val, node_pointer tmp)
	{
		node_pointer new_node = new node(val);

		tmp->right = new_node;
		new_node->parent = tmp;
		new_node->right = this->_last;
		this->_last->parent = new_node;
		++_size;

	}

	void                make_left(const value_type& val, node_pointer tmp)
	{
		node_pointer new_node = new node(val);

		tmp->left = new_node;
		new_node->parent = tmp;
		++_size;

	}

	void                make_right(const value_type& val, node_pointer tmp)
	{
		node_pointer new_node = new node(val);

		tmp->right = new_node;
		new_node->parent = tmp;
		++_size;
	}

	void        		searchSpot(const value_type& val, node_pointer tmp)
	{
		if (val.first == tmp->data.first)
			return ;
		else if (_compare(val.first, tmp->data.first)){
			if (tmp->left == this->_first)
				return make_first(val, tmp);
			if (tmp->left != NULL && tmp->left){
				searchSpot(val, tmp->left);
				return ;
			}
			return make_left(val, tmp);
		}
		if (tmp->right == this->_last)
			return make_last(val, tmp);
		if (tmp->right != NULL && tmp->right != this->_last){
			searchSpot(val, tmp->right);
			return ;
		}
		return make_right(val, tmp);
	}

	void                make_tree(const value_type& val = value_type()){
		this->_root = new node(val);
		this->_root->color = BLACK;

		this->_first = new node();
		this->_last = new node();
		
		this->_root->left = this->_first;
		this->_root->right = this->_last;

		this->_first->parent = _last;
		this->_last->parent = _first;
	}
};
}

#endif
