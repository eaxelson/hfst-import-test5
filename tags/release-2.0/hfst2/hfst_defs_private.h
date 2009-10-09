#ifndef GUARD_HFST_defs_private_h
#define GUARD_HFST_defs_private_h

#define HANDLE_TO_PINSTANCE(aType, aHandle) ((aType*)(aHandle))

#define PINSTANCE_TO_HANDLE(aType, pInstance) ((aType##Handle)(pInstance))

#endif
