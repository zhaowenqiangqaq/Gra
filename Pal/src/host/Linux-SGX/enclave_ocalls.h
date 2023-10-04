/*
 * This is for enclave to make ocalls to untrusted runtime.
 */

#ifndef ENCLAVE_OCALLS_H
#define ENCLAVE_OCALLS_H

#include "pal_linux.h"

#include <asm/stat.h>
#include <linux/socket.h>
#include <linux/poll.h>

noreturn void ocall_exit (int exitcode);

int ocall_print_string (const char * str, unsigned int length);

int ocall_alloc_untrusted (uint64_t size, void ** mem);

int ocall_map_untrusted (int fd, uint64_t offset,
                         uint64_t size, unsigned short prot,
                         void ** mem);

int ocall_unmap_untrusted (const void * mem, uint64_t size);

int ocall_cpuid (unsigned int leaf, unsigned int subleaf,
                 unsigned int values[4]);

int ocall_open (const char * pathname, int flags, unsigned short mode);

int ocall_close (int fd);

int ocall_read (int fd, void * buf, unsigned int count);

int ocall_write (int fd, const void * buf, unsigned int count);

int ocall_fstat (int fd, struct stat * buf);

int ocall_stat (const char * path, struct stat * buf);

int ocall_fionread (int fd);

int ocall_fsetnonblock (int fd, int nonblocking);

int ocall_fchmod (int fd, unsigned short mode);

int ocall_fsync (int fd);

int ocall_ftruncate (int fd, uint64_t length);

int ocall_mkdir (const char *pathname, unsigned short mode);

int ocall_getdents (int fd, struct linux_dirent64 *dirp, unsigned int size);

int ocall_sock_listen (int domain, int type, int protocol,
                       struct sockaddr * addr, unsigned int * addrlen,
                       struct sockopt * opt);

int ocall_sock_accept (int sockfd, struct sockaddr * addr,
                       unsigned int * addrlen, struct sockopt * opt);

int ocall_sock_connect (int domain, int type, int protocol,
                        const struct sockaddr * addr, unsigned int addrlen,
                        struct sockaddr * connaddr,
                        unsigned int * connaddrlen, struct sockopt * opt);

int ocall_sock_recv (int sockfd, void * buf, unsigned int count,
                     struct sockaddr * addr, unsigned int * addrlen);

int ocall_sock_send (int sockfd, const void * buf, unsigned int count,
                     const struct sockaddr * addr, unsigned int addrlen);

int ocall_sock_recv_fd (int sockfd, void * buf, unsigned int count,
                        unsigned int * fds, unsigned int * nfds);

int ocall_sock_send_fd (int sockfd, const void * buf, unsigned int count,
                        const unsigned int * fds, unsigned int nfds);

int ocall_sock_setopt (int sockfd, int level, int optname,
                       const void * optval, unsigned int optlen);

int ocall_sock_shutdown (int sockfd, int how);

int ocall_wake_thread (void * tcs);

int ocall_create_process (const char * uri,
                          int nargs, const char ** args,
                          int procfds[3],
                          unsigned int * pid);

int ocall_futex (int * uaddr, int op, int val, const int64_t * timeout);

int ocall_gettime (unsigned long * microsec);

int ocall_sleep (unsigned long * microsec);

int ocall_socketpair (int domain, int type, int protocol, int sockfds[2]);

int ocall_poll (struct pollfd * fds, int nfds, int64_t * timeout);

int ocall_rename (const char * oldpath, const char * newpath);

int ocall_delete (const char * pathname);

int ocall_load_debug (const char * command);

int ocall_sched_getaffinity(
    unsigned long pid, size_t cpusetsize, unsigned long * mask);

int ocall_sched_setaffinity(
    unsigned long pid, size_t cpusetsize, const unsigned long * mask);

int ocall_rdtsc(unsigned long * low, unsigned long * high);


/* callee saved registers */
struct ocall_marker_buf {
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rsp;
    uint64_t rip;
};

struct ocall_marker_ret {
    int64_t ret;                    /* %rax */
    struct ocall_marker_buf * prev; /* %rdx */
};

struct ocall_marker_ret ocall_marker_save(struct ocall_marker_buf * marker);

static inline struct ocall_marker_buf * ocall_marker_clear(void)
{
    struct ocall_marker_buf * prev = NULL;
    __asm__ volatile (
        "xchgq %0, %%gs:%c1\n"
        : "+r"(prev)
        : "i"(offsetof(struct enclave_tls, ocall_marker))
        : "memory");
    return prev;
}

#endif /* ENCLAVE_OCALLS_H */
