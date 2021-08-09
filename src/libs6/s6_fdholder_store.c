/* ISC license. */

#include <errno.h>

#include <skalibs/allreadwrite.h>
#include <skalibs/tai.h>
#include <skalibs/unixmessage.h>

#include <s6/s6-fdholder.h>

#include <skalibs/posixishard.h>

int s6_fdholder_store (s6_fdholder_t *a, int fd, char const *id, tain const *limit, tain const *deadline, tain *stamp)
{
  unixmessage m ;
  if (!s6_fdholder_store_async(a, fd, id, limit)) return 0 ;
  if (!unixmessage_sender_timed_flush(&a->connection.out, deadline, stamp)) return 0 ;
  if (sanitize_read(unixmessage_timed_receive(&a->connection.in, &m, deadline, stamp)) < 0) return 0 ;
  if (m.len != 1 || m.nfds)
  {
    unixmessage_drop(&m) ;
    return (errno = EPROTO, 0) ;
  }
  return m.s[0] ? (errno = m.s[0], 0) : 1 ;
}
