#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <assert.h>
#include <stddef.h>

#ifdef __GNUC__

struct rb_node {
    unsigned long parent_color;
    struct rb_node *leftC;
    struct rb_node *rightC;
} __attribute__((aligned(sizeof(long))));

#elif __STDC_VERSION__ >= 201112L

#include <stdalign.h>

struct rb_node {
    alignas(sizeof(long)) unsigned long parent_color;
    struct rb_node *leftC;
    struct rb_node *rightC;
};

#else

static_assert(0, "Oops! Version is not support.");

#endif

struct rb_root {
    unsigned long cnt;
    struct rb_node *head;
    struct rb_node nil;
};

#ifndef CONTAINER_OF
#define CONTAINER_OF
#define container_of(ptr, type, member)                                        \
    __extension__({                                                            \
        const __typeof__(((type *)0)->member) *__mptr = (ptr);                 \
        (type *)((char *)__mptr - offsetof(type, member));                     \
    })
#endif

#define RED 0
#define BLACK 1

#define RB_EQUAL 0
#define RB_EQUAL_BREAK -1
#define RB_LEFT 1
#define RB_RIGHT 2

#define rb_parent(r) ((struct rb_node *)((r)->parent_color & ~3))
#define rb_color(r) ((r)->parent_color & 1)

#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) (rb_color(r))

#define rb_set_parent(r, p)                                                    \
    do {                                                                       \
        (r)->parent_color = rb_color(r) | (unsigned long)(p);                  \
    } while (0)

#define rb_set_red(r)                                                          \
    do {                                                                       \
        (r)->parent_color &= ~1;                                               \
    } while (0)

#define rb_set_black(r)                                                        \
    do {                                                                       \
        (r)->parent_color |= 1;                                                \
    } while (0)

#define rb_set_color(d, s)                                                     \
    do {                                                                       \
        struct rb_node __tmp = *(s);                                           \
        __tmp.parent_color |= (unsigned long)~1;                               \
        (d)->parent_color &= __tmp.parent_color;                               \
    } while (0)

#define RB_ROOT_INIT(root)                                                     \
    do {                                                                       \
        (root).head = &((root).nil);                                           \
        (root).nil.parent_color = (unsigned long)BLACK;                        \
        (root).nil.leftC = NULL;                                               \
        (root).nil.rightC = NULL;                                              \
        (root).cnt = 0;                                                        \
    } while (0)

struct rb_node *rbtree_min(struct rb_root *root, struct rb_node *x);
struct rb_node *rbtree_max(struct rb_root *root, struct rb_node *x);

#define rbtree_next(r, n)                                                      \
    ({                                                                         \
        struct rb_root *__r = r;                                               \
        struct rb_node *__tmp, *__n = n;                                       \
        if (__n->rightC != NULL)                                               \
            __tmp = rbtree_min(r, __n->rightC);                                \
        else {                                                                 \
            __tmp = rb_parent(n);                                              \
            while (__tmp != &__r->nil && __n == __tmp->rightC) {               \
                __n = __tmp;                                                   \
                __tmp = rb_parent(__tmp);                                      \
            }                                                                  \
        }                                                                      \
        __tmp;                                                                 \
    })

#define rbtree_prev(r, n)                                                      \
    ({                                                                         \
        struct rb_root *__r = r;                                               \
        struct rb_node *__tmp, *__n = n;                                       \
        if (__n->leftC != NULL)                                                \
            __tmp = rbtree_max(r, __n->leftC);                                 \
        else {                                                                 \
            __tmp = rb_parent(n);                                              \
            while (__tmp != &__r->nil && __n == __tmp->leftC) {                \
                __n = __tmp;                                                   \
                __tmp = rb_parent(__tmp);                                      \
            }                                                                  \
        }                                                                      \
        __tmp;                                                                 \
    })

/*
 * if node1 < node 2 return 1, otherwise return 0.
 */
#define RBTREE_CMP_INSERT_DEFINE(name, rbnode1, rbnode2)                       \
    int name(struct rb_node *rbnode1, struct rb_node *rbnode2)

void rbtree_insert(struct rb_root *root, struct rb_node *node,
                   int (*cmp)(struct rb_node *, struct rb_node *));

void rbtree_clean(struct rb_root *root, void (*freefunc)(struct rb_node *));

/*
 * The return value must in RB_EQUAL, RB_LEFT, RB_RIGHT, RB_EQUAL_BREAK.
 */
#define RBTREE_CMP_SEARCH_DEFINE(name, rbnode, key)                            \
    int name(struct rb_node *rbnode, void *key)

struct rb_node *rbtree_search(struct rb_root *root, void *key,
                              int (*cmp)(struct rb_node *, void *));

#define RBTREE_DELETE_DEFINE(name, rbnode) void name(struct rb_node *rbnode)

void __rbtree_delete(struct rb_root *root, struct rb_node *node);

int rbtree_delete(struct rb_root *root, void *key,
                  int (*cmp)(struct rb_node *, void *),
                  void (*deletefunc)(struct rb_node *));

#endif /* __RBTREE_H__ */