#ifndef DPOR_DECL_H
#define DPOR_DECL_H

#define STRUCT_DECL(type)             \
typedef struct type type;           \
typedef struct type *type##_t;      \
typedef struct type *type##_ref;    \
typedef const struct type *type##_refc;

#define TYPES_DECL(state, ...) \
typedef enum state##_type { __VA_ARGS__ } state##_type; \

#define STATES_DECL(type, ...) \
typedef enum type##_state { __VA_ARGS__ } type##_state; \

#define MEMORY_API_DECL(type) \
type##_ref type##_create(void); \
type##_ref type##_copy(type##_refc); \
void type##_destroy(type##_ref);

#define thread_local __thread

#endif //DPOR_DECL_H