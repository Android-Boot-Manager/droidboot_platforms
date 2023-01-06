#include <errno.h>
#include <stddef.h>
#include <string.h>

/* mips has one error code outside of the 8-bit range due to a
 * historical typo, so we just remap it. */
#if EDQUOT==1133
#define EDQUOT_ORIG 1133
#undef  EDQUOT
#define EDQUOT 109
#endif

static const struct errmsgstr_t {
#define E(n, s) char str##n[sizeof(s)];
#include "__strerror.h"
#undef E
} errmsgstr = {
#define E(n, s) s,
#include "__strerror.h"
#undef E
};

static const unsigned short errmsgidx[] = {
#define E(n, s) [n] = offsetof(struct errmsgstr_t, str##n),
#include "__strerror.h"
#undef E
};

weak_decl char *strerror(int e)
{
	char *s;
	if ((size_t)e >= sizeof errmsgidx / sizeof *errmsgidx) e = 0;
	s = (char *)&errmsgstr + errmsgidx[e];
	return (s);
}
