#ifndef TREE_H
#define TREE_H

#include "../common.h"

#define RB_TREE_EQUAL        0
#define RB_TREE_LESS_THAN    1
#define RB_TREE_GREATER_THAN 2

typedef enum _COLOUR { red, black } COLOUR;

typedef struct _RB_TREE_NODE {
    struct _RB_TREE_NODE* parent;
    struct _RB_TREE_NODE* left;
    struct _RB_TREE_NODE* right;
    COLOUR                colour;
    CHAR                  object[];
} RB_TREE_NODE, *PRB_TREE_NODE;

typedef UINT32 (*RB_COMPARE)(_In_ PVOID Key, _In_ PVOID Object);

typedef struct _RB_TREE {
    PRB_TREE_NODE     root;
    KGUARDED_MUTEX    lock;
    RB_COMPARE        compare;
    LOOKASIDE_LIST_EX pool;
    UINT32            object_size;
    UINT32            active;

    volatile UINT32 node_count;
    volatile UINT32 insertion_count;
    volatile UINT32 deletion_count;

} RB_TREE, *PRB_TREE;

typedef VOID (*RB_CALLBACK)(PRB_TREE_NODE Node);
typedef VOID (*RB_ENUM_CALLBACK)(_In_ PVOID Object, _In_opt_ PVOID Context);

PVOID
RtlRbTreeInsertNode(_In_ PRB_TREE Tree, _In_ PVOID Key);

NTSTATUS
RtlRbTreeCreate(_In_ RB_COMPARE Compare,
                _In_ UINT32     ObjectSize,
                _Out_ PRB_TREE  Tree);

VOID
RtlRbTreeDeleteNode(_In_ PRB_TREE Tree, _In_ PVOID Key);

PVOID
RtlRbTreeFindNodeObject(_In_ PRB_TREE Tree, _In_ PVOID Key);

VOID
RtlRbTreeEnumerate(_In_ PRB_TREE         Tree,
                   _In_ RB_ENUM_CALLBACK Callback,
                   _In_opt_ PVOID        Context);

#define ENUMERATE_THREADS(callback, context) \
    RtlRbTreeEnumerate(GetThreadTree(), callback, context)

VOID
RtlRbTreeDeleteTree(_In_ PRB_TREE Tree);

VOID
RtlRbTreeInOrderPrint(_In_ PRB_TREE Tree);

FORCEINLINE
STATIC
VOID
RtlRbTreeAcquireLock(_Inout_ PRB_TREE Tree)
{
    KeAcquireGuardedMutex(&Tree->lock);
}

FORCEINLINE
STATIC
VOID
RtlRbTreeReleaselock(_Inout_ PRB_TREE Tree)
{
    KeReleaseGuardedMutex(&Tree->lock);
}

VOID
RtlRbTreePrintCurrentStatistics(_In_ PRB_TREE Tree);

#endif