/*
** SCCS ID:	@(#)queues.c	1.1	03/31/09
**
** File:	queues.c
**
** Author:	4003-506 class of 20083
**
** Contributor: Group 5
**
** Description:	queue manipulation types and routines
*/

#define	__KERNEL__20083__

#include "headers.h"

#include "queues.h"

#include "processes.h"

#include "shared_mem.h"

/*
** PRIVATE DEFINITIONS
*/

	// extra qnodes

#define	FUDGE_FACTOR	5

/*
** PRIVATE DATA TYPES
*/

	// queue node structure

typedef struct qnode {
	uint32_t key;
	void *   data;
	struct qnode *next;
} qnode_t;

/*
** PRIVATE GLOBAL VARIABLES
*/

	// queue node set
	// increased number of qnodes to includes shared mem nodes
static qnode_t _qnodes[ 2 * MAX_PROCS + MAX_SHMS + FUDGE_FACTOR ];

	// queue node freelist

static qnode_t *_free_qnodes;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** _qn_alloc()
**
** allocate a queue node
**
** returns the node, or NULL
*/

static qnode_t *_qn_alloc( void ) {
	qnode_t *qnode;

	qnode = _free_qnodes;
	if( qnode ) {
		_free_qnodes = qnode->next;
		qnode->next = 0;
		qnode->data = 0;
		qnode->key = 0;
	}

	return( qnode );

}

/*
** _qn_dealloc(node)
**
** deallocate a queue node, and return it to the free list
*/

static void _qn_dealloc( qnode_t * qnode ) {
	if( qnode ) {
		qnode->next = _free_qnodes;
		_free_qnodes = qnode;
	}
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _q_insert( queue, data )
**
** add 'data' to 'queue'
**
** returns status of the insertion attempt
*/

int _q_insert( queue_t *queue, void *data ) {
	qnode_t *qnode;
	qnode_t *prev, *curr;

	// allocate the queue node

	qnode = _qn_alloc();
	if( !qnode ) {
		return( STAT_ALLOC_FAIL );
	}

	// link the node to the data being inserted

	qnode->data = data;

	// queues can be ordered or FIFO; if ordered, the hashfcn
	// field in the queue header will contain the address of
	// a function which will yield the key for the node being
	// inserted

	if( queue->hashfcn ) {

		// calculate the key

		qnode->key = (*(queue->hashfcn))( data );

		// locate the insertion point

		prev = 0;
		curr = queue->first;

		while( curr && curr->key <= qnode->key ) {
			prev = curr;
			curr = curr->next;
		}

	} else {

		// no hashfcn = insert at the end

		prev = queue->last;
		curr = 0;
	
	}

	// at this point, prev points to the predecessor
	// node and curr points to the successor

	qnode->next = curr;

	// if there is a predecessor, link it to the new
	// node; otherwise, this is the new first node
	// in the queue

	if( prev ) {
		prev->next = qnode;
	} else {
		queue->first = qnode;
	}

	// if there was no successor, this is the
	// new last node in the queue

	if( !curr ) {
		queue->last = qnode;
	}

	// all done!

	return( STAT_SUCCESS );

}

/*
** _q_remove( queue )
**
** remove the first element from 'queue'
**
** returns the removed element, or NULL
*/

void *_q_remove( queue_t *queue ){
	qnode_t *qnode;
	void *data;

	// if there is a node in the queue, remove it

	if( queue->first ) {

		// pull out the first entry
		qnode = queue->first;
		data = qnode->data;

		// unlink this node from the queue

		queue->first = qnode->next;
		if( !queue->first ) {
			// this was the only node in the queue
			queue->last = 0;
		}

		// return the queue node to the free pool

		_qn_dealloc( qnode );

	} else {

		// empty queue - nothing to return

		data = 0;

	}

	return( data );

}

/*
** _q_remove_if_le( queue, key )
**
** if the first item in 'queue' has a key value that is
** less than or equal to the supplied 'key', remove it
**
** returns the removed element, or NULL
*/

void *_q_remove_if_le( queue_t *queue, uint32_t key ) {

	if( queue->first && ((qnode_t *)queue->first)->key <= key ) {
		return( _q_remove(queue) );
	}

	return( 0 );

}

/*
** _q_reset( queue, fcn )
**
** reset 'queue' to its original empty state, using
** 'fcn' as the hash function for ordered insertion
*/

void _q_reset( queue_t *queue, uint32_t (*fcn)(void *) ) {

	if( queue ) {
		queue->hashfcn = fcn;
		queue->first = 0;
		queue->last = 0;
	}

}

/*
** _q_init()
**
** initialize the queue module
*/

void _q_init( void ) {
	int i;

	// start by resetting all the queue nodes

	_memclr( (void *) _qnodes, sizeof(_qnodes) );

	// the free list is initially empty

	_free_qnodes = 0;

	// iterate through the nodes, deallocating each
	// one to add it to the free list

	for( i = 0; i < sizeof(_qnodes)/sizeof(qnode_t); ++i ) {
		_qn_dealloc( &_qnodes[i] );
	}

	// report that this module has been initialized

	c_puts( " queue" );

}

/*
** _hash_sleep(node)
**
** calculate the hash value for 'node', used to
** order the contents of the sleep queue
**
** returns the wakeup time for this process as the hash value
*/

uint32_t _hash_sleep( void *pcb ) {
	
	return( ((pcb_t *)pcb)->wakeup );

}
