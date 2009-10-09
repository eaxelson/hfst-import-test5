#ifndef GUARD_HFST_defs_public_h
#define GUARD_HFST_defs_public_h

#define DECLARE_HANDLE_TYPE(aType)      \
        struct aType##HandleTag { int m_dummy; };  \
        typedef struct aType##HandleTag* aType##Handle

#endif
