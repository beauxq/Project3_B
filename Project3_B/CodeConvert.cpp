#include "CodeConvert.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>  // std::count

#include "Binary_Tree.h"

const char CodeConvert::DOT = '.';  // char used for dot

bool CodeConvert::load_map_from_file(std::map<std::string, CharData>& map_for_tree_creation){

	std::ifstream fin("morse.txt");

	if (! (fin.good() && fin.is_open())){  // if fin didn't open
		// 'morse.txt' file not found.
		return false;
	}

	std::string code;  // code of dots and dashes
	CharData value;    // struct used as value in map_for_tree_creation


	while (fin >> value.character >> code){
        value.dot_count = std::count(code.begin(), code.end(), DOT);
		map_for_tree_creation[code] = value;
		encode_map[value.character] = code;
	}

	fin.close();

	return true;
}

void CodeConvert::create_vector(std::vector<char>& vector_for_tree_creation,
                                const std::map<std::string, CharData>& map_for_tree_creation){
    int previous_code_dot_count = 0;
    int dots_removed_plus_one;

    vector_for_tree_creation.push_back(' ');  // root of tree, has no character (this could be any char)

    for (auto itr = map_for_tree_creation.begin(); itr != map_for_tree_creation.end(); ++itr){
        // add to the vector, nulls, numbering (the number of dots removed from the previous code) + 1
        dots_removed_plus_one = previous_code_dot_count + 1 - (itr->second).dot_count;
        // TODO: error if this number is less than zero? (added more than one dot)
        for (; dots_removed_plus_one > 0; --dots_removed_plus_one){
            vector_for_tree_creation.push_back(0);
        }

        // after adding the appropriate number of nulls, add char to vector
        vector_for_tree_creation.push_back((itr->second).character);

        previous_code_dot_count = (itr->second).dot_count;
    }
}

void CodeConvert::load_from_file(){
    std::map<std::string, CharData> map_for_tree_creation;
    std::vector<char> vector_for_tree_creation;

    load_map_from_file(map_for_tree_creation);
    // TODO: check for return false for file open error

    create_vector(vector_for_tree_creation, map_for_tree_creation);

    // test, TODO: remove tests?
    // does it put it in the right order like we thought it would?
    for (auto itr = vector_for_tree_creation.begin(); itr != vector_for_tree_creation.end(); ++itr)
        std::cout << *itr << std::endl;

    decode_tree.read_tree(vector_for_tree_creation);

    // test
    std:: cout << decode_tree.to_string();

}

std::string CodeConvert::decode(std::string morse_code){
	auto code_itr = morse_code.begin();
	std::string result_string = "";  // return value
	Binary_Tree<char> traversal_tree;

	while (code_itr != morse_code.end()){  // this loop one iteration per decoded character

		traversal_tree = decode_tree; // traversal starts again back at the root of the tree

		while (code_itr != morse_code.end() && !iswspace(*code_itr)){  // this loop one iteration per dot or dash

			// a . means go left in the tree
			// a _ means go right in the tree

			if(*code_itr == DOT){
				try{  // check to see if it actually has a left subtree
					traversal_tree = traversal_tree.get_left_subtree();
					// traversal tree used to be root, but now has a new root (left subtree)
				}
				catch (std::invalid_argument exception){
					std::cerr << "Error: invalid code" << std::endl;
					return "";
				}
			}
			else{  // if it's not a dot or a space, it's a dash
				try{  // similar check for right subtree
					traversal_tree = traversal_tree.get_right_subtree();
				}
				catch (std::invalid_argument exception){
				    std::cerr << "Error: invalid code" << std::endl;
					return "";
				}
			}
			++code_itr;
		}

		if (code_itr != morse_code.end()){
			++code_itr;
		}

		// result_string += whatever char the traversal tree is currently at
		result_string += traversal_tree.get_data();

		//continue until end of morse code
	}
	result_string += '\n'; // end this line since this command is done
	return result_string;
}


std::string CodeConvert::encode(std::string somecharacters){

	std::string resultstring = "";
	auto index = somecharacters.begin();
	while (index != somecharacters.end()){
		if (encode_map.find(*index) == encode_map.end()){
			//error, key not found
			if (iswspace(*index)){
				resultstring += " ";
			}
			else{
				std::cerr << "Error: invalid code" << std::endl;
			}
		}
		else{
			resultstring += encode_map[*index];
		}

		++index;
	}
	resultstring += '\n';// end this line since this command is done
	return resultstring;
}