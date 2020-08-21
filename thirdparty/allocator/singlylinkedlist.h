/////////////////////////////////////////////////////////////////////////////
// Name:         SinglyLinkedList.h
// Description:  ...
// Author:       Mariano Trebino (https://github.com/mtrebi)
// Modified by:  Alexey Orlov (https://github.com/m110h)
// Modified:     08/08/2020
// Licence:      MIT licence
/////////////////////////////////////////////////////////////////////////////

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
