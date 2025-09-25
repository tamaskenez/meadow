#include "meadow/errno.h"

#include "meadow/cppext.h"

string strerrno_or_int(int e)
{
    switch (e) {
#define CASE(X) \
    case X:     \
        return #X;
        CASE(E2BIG)
        CASE(EACCES)
        CASE(EADDRINUSE)
        CASE(EADDRNOTAVAIL)
        CASE(EAFNOSUPPORT)
        CASE(EAGAIN)
        CASE(EALREADY)
        CASE(EBADF)
        CASE(EBADMSG)
        CASE(EBUSY)
        CASE(ECANCELED)
        CASE(ECHILD)
        CASE(ECONNABORTED)
        CASE(ECONNREFUSED)
        CASE(ECONNRESET)
        CASE(EDEADLK)
        CASE(EDESTADDRREQ)
        CASE(EDOM)
        CASE(EEXIST)
        CASE(EFAULT)
        CASE(EFBIG)
        CASE(EHOSTUNREACH)
        CASE(EIDRM)
        CASE(EILSEQ)
        CASE(EINPROGRESS)
        CASE(EINTR)
        CASE(EINVAL)
        CASE(EIO)
        CASE(EISCONN)
        CASE(EISDIR)
        CASE(ELOOP)
        CASE(EMFILE)
        CASE(EMLINK)
        CASE(EMSGSIZE)
        CASE(ENAMETOOLONG)
        CASE(ENETDOWN)
        CASE(ENETRESET)
        CASE(ENETUNREACH)
        CASE(ENFILE)
        CASE(ENOBUFS)
        CASE(ENODATA)
        CASE(ENODEV)
        CASE(ENOENT)
        CASE(ENOEXEC)
        CASE(ENOLCK)
        CASE(ENOLINK)
        CASE(ENOMEM)
        CASE(ENOMSG)
        CASE(ENOPROTOOPT)
        CASE(ENOSPC)
        CASE(ENOSR)
        CASE(ENOSTR)
        CASE(ENOSYS)
        CASE(ENOTCONN)
        CASE(ENOTDIR)
        CASE(ENOTEMPTY)
        CASE(ENOTRECOVERABLE)
        CASE(ENOTSOCK)
#ifdef __EMSCRIPTEN__
        static_assert(ENOTSUP == EOPNOTSUPP);
#else
        CASE(ENOTSUP)
#endif
        CASE(ENOTTY)
        CASE(ENXIO)
        CASE(EOPNOTSUPP)
        CASE(EOVERFLOW)
        CASE(EOWNERDEAD)
        CASE(EPERM)
        CASE(EPIPE)
        CASE(EPROTO)
        CASE(EPROTONOSUPPORT)
        CASE(EPROTOTYPE)
        CASE(ERANGE)
        CASE(EROFS)
        CASE(ESPIPE)
        CASE(ESRCH)
        CASE(ETIME)
        CASE(ETIMEDOUT)
        CASE(ETXTBSY)
        CASE(EXDEV)
#undef CASE
    default:
        if (e == EWOULDBLOCK) // Happens if EAGAIN != EWOULDBLOCK
        {
            return "EWOULDBLOCK";
        }
        return std::to_string(e);
    }
}
