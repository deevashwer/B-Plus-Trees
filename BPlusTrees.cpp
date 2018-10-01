#include <iostream>
#include <vector>

using namespace std;

class Node{
public:
    Node* parent;
    bool leaf;
    bool root;
    vector<int> data;
    vector<Node*> children;
    int order;
    
    Node(){};
    
    Node(int order, Node* parent){
        this->order = order;
        this->children.push_back(NULL);
        this->parent = parent;
        this->leaf = false;
        this->root = false;
    };
    
    Node* search(int value, bool verbose){
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = this->children.begin();
        for(it = this->data.begin(); it != this->data.end(); it++){
            if(verbose)
                cout << '|';
            if(*it > value)
                break;
            if(verbose)
                cout << *it;
            node_it++;
        }
        if(verbose && (node_it + 1) == this->children.end())
            cout << '|';
        if(verbose)
            cout << "->";
        return *node_it;
    }
    
    void insert(int value, Node* ptr){
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = this->children.begin();
        bool flag = false;
        for(it = this->data.begin(); it != this->data.end(); it++){
            node_it++;
            if(*it > value){
                flag = true;
                break;
            }
        }
        if(!flag){
            this->data.push_back(value);
            if(this->leaf)
                this->children.insert(node_it, ptr);
            else
                this->children.push_back(ptr);
        }
        else{
            this->data.insert(it, value);
            this->children.insert(node_it, ptr);
        }
        return;
    }
    
    /*Node* find_left_sibling(int value, int depth, bool found){
        if(depth == 0)
            return this;
        if(found)
            return this->children[this->children.size() - 1]->find_left_sibling(value, depth - 1, true);
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = this->children.begin();
        for(it = this->data.begin(); it != this->data.end(); it++){
            if(*it > value)
                break;
            node_it++;
        }
        if(node_it == this->children.begin())
            return this->parent->find_left_sibling(value, depth + 1, false);
        Node* node = *(node_it - 1);
        return node->find_left_sibling(value, depth - 1, true);
    }
    
    Node* find_right_sibling(int value, int depth, bool found){
        if(depth == 0)
            return this;
        if(found)
            return this->children[0]->find_right_sibling(value, depth - 1, true);
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = this->children.begin();
        for(it = this->data.begin(); it != this->data.end(); it++){
            if(*it > value)
                break;
            node_it++;
        }
        if(node_it == this->children.begin())
            return this->parent->find_right_sibling(value, depth + 1, false);
        Node* node = *(node_it + 1);
        return node->find_right_sibling(value, depth - 1, true);
    }*/
    
    void update(int value){
        if(this->root)
            return;
        Node* parent = this->parent;
        vector<int>::iterator it = parent->data.begin();
        vector<Node*>::iterator node_it;
        if(parent->children[0] != this){
            for(node_it = parent->children.begin() + 1; node_it != parent->children.end(); node_it++){
                if(*node_it == this)
                    break;
                it++;
            }
            *it = value;
            if(this->leaf)
                parent->update(parent->children[0]->data[0]);
            else
                parent->update(value);
        }
        else{
            parent->update(value);
        }
        return;
    }
    
    void print(){
        vector<int>::iterator it;
        cout << '|';
        for(it = this->data.begin(); it != this->data.end(); it++)
            cout << *it << '|';
        return;
    }
    
};

class BPTrees{
public:
    Node* root;
    int order;
    int node_count;
    
    BPTrees(int order){
        this->order = order;
        this->root = new Node(order, NULL);
        this->root->leaf = true;
        this->root->root = true;
        this->node_count = 0;
    }
    
    Node* split(Node* node, int value){
        Node* split_node = new Node(this->order, NULL);
        int mid = (node->order + 1)/2;
        int split_key = node->data[mid];
        vector<int> temp = node->data;
        vector<Node*> temp_ptr = node->children;
        if(node->leaf){
            split_node->leaf = true;
            node->data = vector<int>(temp.begin(), temp.begin() + mid);
            split_node->data = vector<int>(temp.begin() + mid, temp.end());
            node->children = vector<Node*>(temp_ptr.begin(), temp_ptr.begin() + mid);
            split_node->children = vector<Node*>(temp_ptr.begin() + mid, temp_ptr.end());
            node->children.push_back(split_node);
        }
        else{
            if(value == node->data[mid])
                value = node->data[mid + 1];
            node->data = vector<int>(temp.begin(), temp.begin() + mid);
            split_node->data = vector<int>(temp.begin() + mid + 1, temp.end());
            node->children = vector<Node*>(temp_ptr.begin(), temp_ptr.begin() + mid + 1);
            split_node->children = vector<Node*>(temp_ptr.begin() + mid + 1, temp_ptr.end());
        }
        int tmp = node->children.size() - 1;
        if(!node->leaf)
            tmp++;
        for(int i = 0; i < tmp; i++)
            if(node->children[i] != NULL)
                node->children[i]->parent = node;
        tmp = split_node->children.size() - 1;
        if(!split_node->leaf)
            tmp++;
        for(int i = 0; i < tmp; i++)
            if(split_node->children[i] != NULL)
                split_node->children[i]->parent = split_node;
        if(node->root){
            node->root = false;
            this->root = new Node(this->order, NULL);
            this->root->root = true;
            this->root->data.push_back(split_key);
            this->root->children[0] = node;
            this->root->children.push_back(split_node);
            node->parent = split_node->parent = this->root;
        }
        else{
            split_node->parent = node->parent;
            node->parent->insert(split_key, split_node);
            if(node->parent->data.size() == (this->order + 1))
                split_node->parent = this->split(node->parent, split_key);
        }
        if(value >= split_node->data[0])
            return split_node;
        else
            return node;
    }
    
    bool search(int value){
        Node* node = this->root;
        while(!node->leaf)
            node = node->search(value, true);
        vector<int>::iterator it;
        cout << '|';
        for(it = node->data.begin(); it != node->data.end(); it++)
            if(*it >= value)
                break;
            else
                cout << *it << '|';
        if(it == node->data.end() || *it != value){
            cout << "X" << endl;
            return false;
        }
        cout << value << endl;
        /*node = node->parent;
        while(node != NULL){
            node->search(value, true);
            node = node->parent;
        }
        cout << endl;*/
        return true;
    }

    bool insert(int value){
        Node* node = this->root;
        while(!node->leaf)
            node = node->search(value, false);
        vector<int>::iterator it;
        for(it = node->data.begin(); it != node->data.end(); it++)
            if(*it == value)
                return false;
        node->insert(value, NULL);
        if(node->data.size() == (this->order + 1))
            this->split(node, value);
        return true;
    }
    
    void remove_internal(Node* node, int value){
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = node->children.begin();
        for(it = node->data.begin(); it != node->data.end(); it++){
            if(*it > value)
                break;
            node_it++;
        }
        it--;
        node->data.erase(it);
        node->children.erase(node_it);
        while(true){
            if(node->data.size() == 0  && node->root){
                this->root = node->children[0];
                this->root->root = true;
                this->root->parent = NULL;
                return;
            }
            if(node == this->root || node->data.size() >= (this->order - 1)/2)
                return;
            else{
                Node* sibling;
                Node* parent = node->parent;
                if(parent->children[0] == node){
                    sibling = parent->children[1];
                    if(sibling->data.size() > (this->order - 1)/2){
                        node->data.insert(node->data.end(), parent->data[0]);
                        node->children.insert(node->children.end(), sibling->children[0]);
                        node->children[node->children.size() - 1]->parent = node;
                        sibling->data.erase(sibling->data.begin());
                        sibling->children.erase(sibling->children.begin());
                        parent->data[0] = sibling->data[0];
                        return;
                    }
                    node->data.push_back(parent->data[0]);
                    node->children.push_back(sibling->children[0]);
                    node->children[sibling->children.size() - 1]->parent = node;
                    node->data.insert(node->data.end(), sibling->data.begin(), sibling->data.end());
                    parent->data.erase(parent->data.begin());
                    parent->children.erase(parent->children.begin());
                    for(node_it = sibling->children.begin(); node_it != sibling->children.end(); node_it++)
                        (*node_it)->parent = node;
                    node->children.insert(node->children.end(), sibling->children.begin(), sibling->children.end());
                    delete sibling;
                    node = parent;
                    continue;
                }
                else{
                    for(node_it = parent->children.begin(); node_it != parent->children.end(); node_it++)
                        if(*node_it == node)
                            break;
                    node_it--;
                    sibling = *node_it;
                    node_it++;
                    for(it = parent->data.begin(); it != parent->data.end(); it++)
                        if(*it > value)
                            break;
                    it--;
                    if(sibling->data.size() > (this->order - 1)/2){
                        node->data.insert(node->data.begin(), *it);
                        node->children.insert(node->children.begin(), sibling->children[sibling->children.size() - 1]);
                        node->children[0]->parent = node;
                        sibling->data.pop_back();
                        sibling->children.pop_back();
                        *it = sibling->data[sibling->data.size() - 1];
                        return;
                    }
                    sibling->data.push_back(*it);
                    sibling->children.push_back(node->children[0]);
                    sibling->children[sibling->children.size() - 1]->parent = sibling;
                    sibling->data.insert(sibling->data.end(), node->data.begin(), node->data.end());
                    parent->data.erase(it);
                    parent->children.erase(node_it);
                    for(node_it = node->children.begin(); node_it != node->children.end(); node_it++)
                        (*node_it)->parent = sibling;
                    sibling->children.insert(sibling->children.end(), node->children.begin(), node->children.end());
                    delete node;
                    node = parent;
                    continue;
                }
            }
        }
        return;
    }
    
    void remove(int value){
        Node* node = this->root;
        while(!node->leaf)
            node = node->search(value, false);
        vector<int>::iterator it;
        vector<Node*>::iterator node_it = node->children.begin();
        for(it = node->data.begin(); it != node->data.end(); it++){
            if(*it == value)
                break;
            node_it++;
        }
        if(it == node->data.end())
            return;
        node->data.erase(it);
        node->children.erase(node_it);
        if(node->root || node->data.size() >= (this->order - 1)/2){
            if(it == node->data.begin())
                node->update(node->data[0]);
            return;
        }
        else{
            Node* sibling;
            Node* parent = node->parent;
            if(parent->children[0] == node){
                sibling = parent->children[1];
                if(sibling->data.size() > (this->order - 1)/2){
                    node->data.push_back(sibling->data[0]);
                    node->children.insert(node->children.begin(), NULL);
                    sibling->data.erase(sibling->data.begin());
                    sibling->children.erase(sibling->children.begin());
                    sibling->update(sibling->data[0]);
                    return;
                }
                value = sibling->data[0];
                node->data.insert(node->data.end(), sibling->data.begin(), sibling->data.end());
                node->children.pop_back();
                node->children.insert(node->children.end(), sibling->children.begin(), sibling->children.end());
                sibling->update(value);
                delete sibling;
                this->remove_internal(parent, value);
            }
            else{
                for(node_it = parent->children.begin(); node_it != parent->children.end(); node_it++)
                    if(*node_it == node)
                        break;
                node_it--;
                sibling = *node_it;
                node_it++;
                if(sibling->data.size() > (this->order - 1)/2){
                    node->data.insert(node->data.begin(), sibling->data[sibling->data.size() - 1]);
                    node->children.insert(node->children.begin(), NULL);
                    sibling->data.pop_back();
                    sibling->children.erase(sibling->children.begin());
                    node->update(node->data[0]);
                    return;
                }
                sibling->data.insert(sibling->data.end(), node->data.begin(), node->data.end());
                sibling->children.pop_back();
                sibling->children.insert(sibling->children.end(), node->children.begin(), node->children.end());
                delete node;
                this->remove_internal(parent, value);
            }
        }
    }
    
    void print(){
        return;
    }
};

int main(int argc, const char * argv[]) {
    /*cout << "Help:\n----\n[i]nsert\n[p]rint tree\n[h]elp\n[q]uit\n" << endl;
    cout << "Input Tree Order: ";
    int order;
    cin >> order;
    BPTrees* tree = new BPTrees(order);
    char command;
    int value;
    while(true){
        cout << "Input a command: ";
        cin >> command;
        switch(command){
            case 'i':
                cout << "<value>: ";
                cin >> value;
                tree->insert(value);
                break;
            case 'p':
                cout << tree << endl;
                break;
            case 'h':
                cout << "Help:\n----\n[i]nsert\n[p]rint tree\n[h]elp\n[q]uit\n" << std::endl;
                break;
            case 'q':
                return 0;
                break;
            default:
                std::cout << "Invalid Command" << std::endl;
        }
    }*/
    BPTrees* tree = new BPTrees(3);
    vector<int> values;
    int temp;
    for(int i = 0; i < 10; i++){
        temp = rand() % 100;
        values.push_back(temp);
        tree->insert(temp);
        /*cout << i << " " << temp << endl;
        for(int j = 0; j < values.size(); j++){
            cout << values[j] << " ";
            if(!tree->search(values[j]))
                return 0;
        }
        cout << endl;*/
    }
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(58);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(23);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(9);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(30);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(44);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(72);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(73);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(49);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(78);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    tree->remove(7);
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    cout << endl;
    for(int i = 0; i < 10; i++)
        tree->search(values[i]);
    /*tree->insert(7);
    tree->insert(49);
    tree->insert(73);
    tree->insert(58);
    tree->insert(30);
    tree->insert(72);
    tree->insert(44);
    tree->insert(78);
    tree->insert(23);
    tree->insert(9);
    tree->insert(40);
    tree->insert(65);
    tree->insert(92);
    tree->insert(42);
    tree->insert(87);
    tree->insert(3);
    tree->insert(27);
    tree->insert(29);
    tree->insert(12);
    tree->insert(69);
    tree->insert(57);
    tree->insert(60);
    tree->insert(33);
    tree->insert(99);
    tree->insert(16);
    tree->insert(35);
    tree->insert(97);
    tree->insert(26);
    tree->insert(67);
    tree->insert(10);
    tree->insert(79);
    tree->insert(49);
    tree->insert(21);*/
    /*
    tree->insert(67);
    tree->insert(72);
    tree->insert(93);
    tree->insert(36);
    tree->insert(85);
    tree->insert(45);*/
    return 0;
}
