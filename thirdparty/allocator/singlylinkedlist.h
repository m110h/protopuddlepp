#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

namespace mtrebi
{

template <class T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node* next {nullptr};
    };

    Node* head {nullptr};

public:
    SinglyLinkedList();

    void insert(Node* previousNode, Node* newNode);
    void remove(Node* previousNode, Node* deleteNode);
};

}

#include "singlylinkedlistimpl.h"

#endif /* SINGLYLINKEDLIST_H */

