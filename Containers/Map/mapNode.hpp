/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   mapNode.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/04/09 11:42:25 by wester        #+#    #+#                 */
/*   Updated: 2021/04/15 22:41:14 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAPNODE_HPP
# define MAPNODE_HPP

# include <algorithm>
# include <cstddef>
# include <iostream>

enum color{
	BLACK = 0,
	RED = 1
};

namespace ft 
{
template<typename T>
class mapNode 
{
	public:
		typedef T       value_type;
		
		value_type      data;
		mapNode*        left;
		mapNode*        right;
		mapNode*        parent;
		bool			color;
	
		explicit mapNode(): data(), left(nullptr), right(nullptr), parent(nullptr), color(BLACK)
		{};
		
		explicit mapNode(const T& data): data(data), left(nullptr), right(nullptr), parent(nullptr), color(RED)
		{};
		
		mapNode(const mapNode& other){ *this = other; };
		mapNode&   operator=(const mapNode& other){
				this->data = other.data;
				this->left = other.left;
				this->right = other.right;
				this->parent = other.parent;
				this->color = other.color;
				return *this;
		}

		~mapNode(){};

		// bool        operator==(const mapNode& other) const{
		// 	if (this->data == other.data && this->left == other.left && this->right == other.right && this->parent == other.parent)
		// 		return true;
		// 	return false;
		// }
		// bool        operator!=(const mapNode& other) const{
		// 	return (!(*this == other));
		// }
		
		mapNode*   getnext() {
			mapNode* tmp = this;
			if (right == NULL){
				while (tmp->parent != NULL && tmp == tmp->parent->right)
					tmp = tmp->parent;
				return tmp->parent;
			}
			tmp = tmp->right;
			while (tmp->left != NULL)
				tmp = tmp->left;
			return tmp; 
		};
		
    	mapNode*   getprevious() {
			mapNode* tmp = this;
			if (left == NULL){
				while (tmp->parent != NULL && tmp == tmp->parent->left)
					tmp = tmp->parent;
				return tmp->parent;
			}
			tmp = tmp->left;
			while (tmp->right != NULL)
				tmp = tmp->right;
			return tmp; 
		};
};
}

#endif
		