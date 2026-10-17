/*
 * static_containers.h - vector and associative array on static memory.
 * No malloc, no dependencies besides the standard library (memmove).
 *
 * Places meant for customization are marked: [CUSTOM 1..3]
 */
#ifndef static_containers_h
#define static_containers_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ------------------------------------------------------------------
 * [CUSTOM 1] Key comparison functions for the map.
 * Any macro/function of the form int cmp(a, b): returns <0, 0, >0.
 * A ready-made one for numbers/chars/pointers is below.
 * For strings use strcmp, for your own types write your own.
 * ------------------------------------------------------------------ */
#define cmp_scalar(a, b) (((a) > (b)) - ((a) < (b)))

/* ------------------------------------------------------------------
 * Vector.
 * define_static_vec(name, element_type, capacity, counter_type)
 *
 * [CUSTOM 2] counter_type: uint8_t (up to 255 elements), uint16_t, etc.
 *            Saves RAM on an MCU. capacity must fit into this type.
 * [CUSTOM 3] element_type: any type, including a struct (it is copied
 *            by value; for large structs it is often better to modify
 *            the element in place through get()).
 * ------------------------------------------------------------------ */
#define define_static_vec(name, type, cap, len_type) \
    typedef struct { \
        type data[cap]; \
        len_type len; \
    } name; \
    \
    static inline void name##_clear(name *v) { \
        v->len = 0; \
    } \
    \
    static inline type *name##_get(name *v, size_t i) { \
        return i < v->len ? &v->data[i] : NULL; \
    } \
    \
    static inline bool name##_push(name *v, type item) { \
        if (v->len >= (cap)) { \
            return false; \
        } \
        v->data[v->len++] = item; \
        return true; \
    } \
    \
    static inline bool name##_pop(name *v, type *out) { \
        if (v->len == 0) { \
            return false; \
        } \
        v->len--; \
        if (out) { \
            *out = v->data[v->len]; \
        } \
        return true; \
    } \
    \
    static inline bool name##_insert(name *v, size_t i, type item) { \
        if (v->len >= (cap) || i > v->len) { \
            return false; \
        } \
        memmove(&v->data[i + 1], &v->data[i], (v->len - i) * sizeof(type)); \
        v->data[i] = item; \
        v->len++; \
        return true; \
    } \
    \
    static inline bool name##_remove(name *v, size_t i) { \
        if (i >= v->len) { \
            return false; \
        } \
        memmove(&v->data[i], &v->data[i + 1], (v->len - i - 1) * sizeof(type)); \
        v->len--; \
        return true; \
    }

/* ------------------------------------------------------------------
 * Associative array: a sorted array of pairs + binary search.
 * define_static_map(name, key_type, value_type, capacity, counter_type, cmp)
 *
 * Why this design: there are no empty slots and no load-factor headroom
 * (saves RAM), lookup is O(log n), insertion and removal are a plain
 * shift via memmove. For an MCU with tens to hundreds of entries this is
 * faster and more compact than a hash table.
 *
 * cmp - see [CUSTOM 1]. The key is passed to cmp by value.
 * A pointer key (const char *) must outlive the map.
 * ------------------------------------------------------------------ */
#define define_static_map(name, key_type, val_type, cap, len_type, key_cmp) \
    typedef struct { \
        key_type key; \
        val_type value; \
    } name##_entry; \
    \
    typedef struct { \
        name##_entry items[cap]; \
        len_type len; \
    } name; \
    \
    static inline void name##_clear(name *m) { \
        m->len = 0; \
    } \
    \
    /* first index whose key is >= the searched key */ \
    static inline size_t name##_lower_bound(const name *m, key_type key) { \
        size_t lo = 0; \
        size_t hi = m->len; \
        while (lo < hi) { \
            size_t mid = lo + (hi - lo) / 2; \
            if (key_cmp(m->items[mid].key, key) < 0) { \
                lo = mid + 1; \
            } else { \
                hi = mid; \
            } \
        } \
        return lo; \
    } \
    \
    static inline val_type *name##_get(name *m, key_type key) { \
        size_t i = name##_lower_bound(m, key); \
        if (i < m->len && key_cmp(m->items[i].key, key) == 0) { \
            return &m->items[i].value; \
        } \
        return NULL; \
    } \
    \
    static inline bool name##_put(name *m, key_type key, val_type value) { \
        size_t i = name##_lower_bound(m, key); \
        if (i < m->len && key_cmp(m->items[i].key, key) == 0) { \
            m->items[i].value = value; \
            return true; \
        } \
        if (m->len >= (cap)) { \
            return false; \
        } \
        memmove(&m->items[i + 1], &m->items[i], (m->len - i) * sizeof(name##_entry)); \
        m->items[i].key = key; \
        m->items[i].value = value; \
        m->len++; \
        return true; \
    } \
    \
    static inline bool name##_remove(name *m, key_type key) { \
        size_t i = name##_lower_bound(m, key); \
        if (i >= m->len || key_cmp(m->items[i].key, key) != 0) { \
            return false; \
        } \
        memmove(&m->items[i], &m->items[i + 1], (m->len - i - 1) * sizeof(name##_entry)); \
        m->len--; \
        return true; \
    }

#endif /* static_containers_h */
