#ifndef DDS_CPP_H
#define DDS_CPP_H

#ifdef __cplusplus
extern "C" {
#endif
#include <os_if.h>

/* if you want to build the C preprocessor as a dll, please define:
#define OSPL_BUILD_CPP_SHARED
*/
#ifdef OSPL_BULD_CPP_SHARED

#ifdef OSPL_BUILD_CPP
#define OS_API OS_API_EXPORT
#else
#define OS_API OS_API_IMPORT
#endif /* OSPL_BUILD_CPP */
/* !!!!!!!!NOTE From here no more includes are allowed!!!!!!! */

#else /* !defined OSPL_BULD_CPP_SHARED */

#define OS_API

#endif /* OSPL_BUILD_CPP_SHARED */


#define DEF_CMDLINE 1

OS_API void init_preprocess(void);
OS_API void preprocess(FILE *infile, char *infilename);
OS_API int preprocess_getc(void);
OS_API void Ifile (char *);
OS_API void define (char * name, int nargs, unsigned char * repl, int how);

#undef OS_API

#ifdef __cplusplus
}
#endif

#endif /* DDS_CPP_H */