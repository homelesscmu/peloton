### Paper: Concurrent Maintenance of Skip Lists

This paper should be the first to discuss concurrency in skip lists. But since it was written in 1990, the techniques might be irrelevant today ... 

They have a section discussing and proving an algorithm on concurrent sorted linked list. Here is some interesting points:

* They use write locks, so the method **is not directly applicable**.
* The novel idea in our method is the use of pointer reversal. When deleting a node x, we update the predecessor of x to point to the successor of x, in the standard way. But we also update x itself so as to point back to its former predecessor. Other threads that were passing through x at the time of its deletion can follow this reversed pointed to get back into the current list, at the correct place to continue their work.
* When we delete a node y, we cannot immediately garbage collect y because other threads may have a pointer to y. Instead, putOnGarbageQueue(y) puts y onto a garbage queue. A node can be taken off the garbage queue any time after the completion of all searches/insertions/deletions that were in progress when the node was put on the queue.


### Resource: Lock-Free Linked List

**The code may be worth looking into, for our reference.** Below is some of the key design ideas I found in their documentations.

#### Traversal (no problem)

If insert – there is always a path for the traversing process.  
If delete – there is always a path – the deleted node maintains a forward path – In a garbage collection system, that node will continue to exist until there are no more pointers to it.

#### Insert (no problem with CAS)

INPUT: p -> s, OUTPUT: p -> q -> s  

Say we want to insert a new node q between p and s.   

q next points to s  
p next points to q  

BUT – if another process deletes node s in between, then q will be following a deleted node.  
THEREFORE - (swing pointer using Compare&Swap)  
To do this - Set p->next to point to q, in one atomic statement, only if p has not changed (compare) since p was last read.  


#### Delete (big problem)

INPUT: a -> b -> d, OUTPUT: a -> c -> d

Simple delete looks like this.  
	a->next = a->next->next  

BUT – what happens if there is a concurrent insert?  
We are going to delete b while inserting c.  

Delete (with concurrent insert):  

1 -	C next = d (c insert)  
2 - 	a next = d (b delete) (cursor also moves)  
3 - 	b next = c (c insert) pointed at by a deleted node  

To solve this problem, we use auxiliary nodes. An auxiliary node is a node with only a next pointer, no data. We put an auxiliary node in between each cell in the list. (Then the steps get complicated ... refer to PPT page 21-25 for illustration)
