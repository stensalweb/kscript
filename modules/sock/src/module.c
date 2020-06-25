/* sock/src/module.c - the kscript's requests library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "sock"

// include this since this is a module.
#include "ks-module.h"


// system socket library
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <errno.h>

// sock.Socket - class describing a Socket object
typedef struct {
    KS_OBJ_BASE

    // socket descriptor (similar to a FILE descriptor)
    int sockfd;

    // the address family type (i.e. AF_INET most of the time)
    int af_type;

    // the socket type (i.e. SOCK_STREAM for TCP)
    int socket_type;

    // whether or not it is currently bound and whether it is currently listening
    bool is_bound, is_listening, is_connected;

    // the address of the socket (only valid when is_bound)
    struct sockaddr_in sa_addr;


}* sock_Socket;


// declare the array type as residing in this file
KS_TYPE_DECLFWD(sock_type_Socket);


// create a new socket
static sock_Socket make_Socket() {
    sock_Socket self = KS_ALLOC_OBJ(sock_Socket);
    KS_INIT_OBJ(self, sock_type_Socket);

    // not currently bound or listening
    self->is_bound = false;
    self->is_listening = false;

    // AF_INET=IPv4, AF_INET6=IPv6
    self->af_type = AF_INET;

    // SOCK_STREAM=TCP, SOCK_DGRAM=UDP
    self->socket_type = SOCK_STREAM;

    // create file descriptor
    self->sockfd = socket(self->af_type, self->socket_type, 0 /* 0=automatic protocol */);

    // check for error after opening the socket
    if (self->sockfd < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket creation failed! (reason: %s)", strerror(errno));
    }

    // socket option argument; must be variable due to pass-by-address
    int opt = 1;

    // set options to re-use the addresses & ports within the processes
    if (setsockopt(self->sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt)) < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket option setting (SO_REUSEADDR) failed! (reason: %s)", strerror(errno));
    }

    // if SO_REUSEPORT is defined, try and set it
    #ifdef SO_REUSEPORT
    if(setsockopt(self->sockfd, SOL_SOCKET, SO_REUSEPORT, (void*)&opt, sizeof(opt)) < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket option setting (SO_REUSEPORT) failed! (reason: %s)", strerror(errno));
    }
    #endif

    return self;
}

// create a new socket, just wrapping existing resources
static sock_Socket wrap_Socket(int sockfd, int af_type, int socket_type, bool is_bound, bool is_listening) {
    sock_Socket self = KS_ALLOC_OBJ(sock_Socket);
    KS_INIT_OBJ(self, sock_type_Socket);

    // not currently bound or listening
    self->is_bound = is_bound;
    self->is_listening = is_listening;

    // AF_INET=IPv4, AF_INET6=IPv6
    self->af_type = af_type;

    // SOCK_STREAM=TCP, SOCK_DGRAM=UDP
    self->socket_type = socket_type;

    // create file descriptor
    self->sockfd = sockfd;

    // check for error after opening the socket
    if (self->sockfd < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket creation failed! (reason: %s)", strerror(errno));
    }

    // socket option argument; must be variable due to pass-by-address
    int opt = 1;

    // set options to re-use the addresses & ports within the processes
    if (setsockopt(self->sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt)) < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket option setting (SO_REUSEADDR) failed! (reason: %s)", strerror(errno));
    }

    // if SO_REUSEPORT is defined, try and set it
    #ifdef SO_REUSEPORT
    if(setsockopt(self->sockfd, SOL_SOCKET, SO_REUSEPORT, (void*)&opt, sizeof(opt)) < 0) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_IOError, "Socket option setting (SO_REUSEPORT) failed! (reason: %s)", strerror(errno));
    }
    #endif

    return self;
}


/* Socket.__new__(domain)
 *
 * Create a Socket object
 * 
 */
static KS_TFUNC(Socket, new) {
    KS_REQ_N_ARGS(n_args, 0);

    return (ks_obj)make_Socket();
}

/* Socket.__free__(self) 
 *
 * Free a Socket object, and close it
 * 
 */
static KS_TFUNC(Socket, free) {
    KS_REQ_N_ARGS(n_args, 1);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");

    // if the socket is valid, close it down
    if (self->sockfd >= 0) close(self->sockfd);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* Socket.connect(self, address, port)
 *
 * Connect a socket object to an address & port
 * 
 */
static KS_TFUNC(Socket, connect) {
    KS_REQ_N_ARGS(n_args, 3);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");
    ks_str address = (ks_str)args[1];
    KS_REQ_TYPE(address, ks_type_str, "address");
    ks_int port = (ks_int)args[2];
    KS_REQ_TYPE(port, ks_type_int, "port");

    // the server address
    struct sockaddr_in serv_addr;

    // set the family type to the AF type that the socket was created with
    serv_addr.sin_family = self->af_type;

    // set the port as well
    serv_addr.sin_port = htons((uint16_t)port->val);

    // try to resolve the address
    // (TODO: perhaps make this a function?)
    if (ks_str_cmp_c(address, "localhost") == 0 || ks_str_cmp_c(address, "") == 0) {
        serv_addr.sin_addr.s_addr = INADDR_ANY;
    } else if (inet_pton(self->af_type, address->chr, &serv_addr.sin_addr) <= 0) {
        return ks_throw_fmt(ks_type_IOError, "Could not resolve address given ('%S')", address);
    }

    // attempt to connect it
    if (connect(self->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  { 
        return ks_throw_fmt(ks_type_IOError, "Could not connect socket! (reason: %s)", strerror(errno));
    } 

    // set it to currently bound
    self->is_connected = true;
    
    return KSO_NONE;
}

/* Socket.bind(self, address, port)
 *
 * Bind a socket object to an address & port
 * 
 */
static KS_TFUNC(Socket, bind) {
    KS_REQ_N_ARGS(n_args, 3);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");
    ks_str address = (ks_str)args[1];
    KS_REQ_TYPE(address, ks_type_str, "address");
    ks_int port = (ks_int)args[2];
    KS_REQ_TYPE(port, ks_type_int, "port");

    // set the family type to the AF type that the socket was created with
    self->sa_addr.sin_family = self->af_type;

    // set the port as well
    self->sa_addr.sin_port = htons((uint16_t)port->val);

    // try to resolve the address
    // (TODO: perhaps make this a function?)
    if (ks_str_cmp_c(address, "localhost") == 0 || ks_str_cmp_c(address, "") == 0) {
        self->sa_addr.sin_addr.s_addr = INADDR_ANY;
    } else if (inet_pton(self->af_type, address->chr, &self->sa_addr.sin_addr) <= 0) {
        return ks_throw_fmt(ks_type_IOError, "Could not resolve address given ('%S')", address);
    }

    // attempt to bind the sockfd to the given address
    if (bind(self->sockfd, (struct sockaddr *)&self->sa_addr, sizeof(self->sa_addr)) < 0) {
        return ks_throw_fmt(ks_type_IOError, "Could not bind socket! (reason: %s)", strerror(errno));
    } 

    // set it to currently bound
    self->is_bound = true;
    
    return KSO_NONE;
}


/* Socket.listen(self, num)
 *
 * Have a socket listen to a given number of connections
 * 
 */
static KS_TFUNC(Socket, listen) {
    KS_REQ_N_ARGS(n_args, 2);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");
    ks_int num = (ks_int)args[1];
    KS_REQ_TYPE(num, ks_type_int, "num");

    // make sure it's valid
    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Cant listen before the socket is bound!");
    }

    // attempt to listen to that number of connections at a time
    if (listen(self->sockfd, num->val) < 0) { 
        return ks_throw_fmt(ks_type_IOError, "Could not listen on socket! (reason: %s)", strerror(errno));
    }
    
    // set it to currently listening
    self->is_listening = true;
    
    return KSO_NONE;
}

/* Socket.accept(self)
 *
 * Accepts a new connection, blocking until it finds one
 * 
 */
static KS_TFUNC(Socket, accept) {
    KS_REQ_N_ARGS(n_args, 1);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");

    // make sure it's valid
    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Cant accept before the socket is bound!");
    }

    // make sure it's valid
    if (!self->is_listening) {
        return ks_throw_fmt(ks_type_IOError, "Cant accept before the socket is listening!");
    }

    // data about the incoming connection
    int sockfd_conn;
    struct sockaddr_in addr_conn;
    socklen_t addr_conn_len = sizeof(addr_conn);

    // search for a connection
    if ((sockfd_conn = accept(self->sockfd, (struct sockaddr*)&addr_conn, &addr_conn_len)) < 0) {
        return ks_throw_fmt(ks_type_IOError, "Failed to accept new connection (reason: %s)", strerror(errno));
    }

    // maximum address string length we wish to support
    #define MAX_ADDRSTRLEN 256
    char tmpstr[MAX_ADDRSTRLEN];

    if (!inet_ntop(addr_conn.sin_family, &addr_conn.sin_addr, tmpstr, MAX_ADDRSTRLEN)) {
        return ks_throw_fmt(ks_type_IOError, "Could not get name for socket connection (reason: %s)", strerror(errno));
    }

    // construct a tuple with (socket, address)
    return ks_tuple_new_n(2, (ks_obj[]){ wrap_Socket(sockfd_conn, addr_conn.sin_family, self->socket_type, true, false), ks_str_new(tmpstr) });
}



/* Socket.send(self, msg)
 *
 * Sends a given message (which is converted to a string if it is not already one)
 * 
 */
static KS_TFUNC(Socket, send) {
    KS_REQ_N_ARGS(n_args, 2);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");
    ks_obj msg_obj = args[1];
    ks_str msg_str = NULL;

    // make sure it's valid
    if (!self->is_bound && !self->is_connected) {
        return ks_throw_fmt(ks_type_IOError, "Cant send before the socket is bound!");
    }

    // ensure we have a string
    if (msg_obj->type == ks_type_str) {
        msg_str = (ks_str)KS_NEWREF(msg_obj);
    } else {
        msg_str = ks_fmt_c("%S", msg_obj);
        if (!msg_str) return NULL;
    }

    ssize_t total_sent = 0;
    do {
        // actually send bytes
        ssize_t actual_sz = send(self->sockfd, &msg_str->chr[total_sent], msg_str->len - total_sent, 0);

        // check and ensure message went through
        if (actual_sz < 0) {
            KS_DECREF(msg_str);
            return ks_throw_fmt(ks_type_IOError, "Could not send from socket (reason: %s)", strerror(errno));
        }


        total_sent += actual_sz;

    } while (total_sent < msg_str->len);


    // remove created string reference
    KS_DECREF(msg_str);


    return KSO_NONE;
}



/* Socket.recv(self, sz)
 *
 * Recieves a given size of data (as a string)
 * 
 */
static KS_TFUNC(Socket, recv) {
    KS_REQ_N_ARGS(n_args, 2);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");
    ks_int sz = (ks_int)args[1];
    KS_REQ_TYPE(sz, ks_type_int, "sz");

    // make sure it's valid
    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Cant recv before the socket is bound!");
    }

    void* tmpbuf = ks_malloc(sz->val);
    ssize_t sum_sz = 0;


    do {
        ssize_t actual_sz = recv(self->sockfd, &tmpbuf[sum_sz], sz->val - sum_sz, 0);

        if (actual_sz < 0) {
            ks_free(tmpbuf);
            return ks_throw_fmt(ks_type_IOError, "Could not recv into socket (reason: %s)", strerror(errno));
        }

        sum_sz += actual_sz;

    } while (sum_sz < sz->val);


    // convert to a string
    ks_str res = ks_str_new_l(tmpbuf, sum_sz);
    ks_free(tmpbuf);

    return (ks_obj)res;
}



/* Socket.get_name(self)
 *
 * Return a textual name for the socket
 * 
 */
static KS_TFUNC(Socket, get_name) {
    KS_REQ_N_ARGS(n_args, 1);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");

    // make sure it's valid
    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Cant get the name before the socket is bound!");
    }

    // maximum address string length we wish to support
    #define MAX_ADDRSTRLEN 256
    char tmpstr[MAX_ADDRSTRLEN];

    if (!inet_ntop(self->af_type, &self->sa_addr.sin_addr, tmpstr, MAX_ADDRSTRLEN)) {
        return ks_throw_fmt(ks_type_IOError, "Could not get name for socket (reason: %s)", strerror(errno));
    }

    return (ks_obj)ks_str_new(tmpstr);
}

/* Socket.get_port(self)
 *
 * Return an integer describing the port of the socket
 * 
 */
static KS_TFUNC(Socket, get_port) {
    KS_REQ_N_ARGS(n_args, 1);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");

    // make sure it's valid
    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Cant get the port before the socket is bound!");
    }

    return (ks_int)ks_int_new((int)ntohs(self->sa_addr.sin_port));
}

/* Socket.handle_forever(self)
 *
 * Bind a socket object to an address & port
 * 
 */
static KS_TFUNC(Socket, handle_forever) {
    KS_REQ_N_ARGS(n_args, 1);
    sock_Socket self = (sock_Socket)args[0];
    KS_REQ_TYPE(self, sock_type_Socket, "self");

    if (!self->is_bound) {
        return ks_throw_fmt(ks_type_IOError, "Attempted to listen without being bound");
    }

    if (listen (self->sockfd, 1) != 0) {
        return ks_throw_fmt(ks_type_IOError, "Could not open! (reason: %s)", strerror(errno));
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &self->sa_addr.sin_addr, str, INET_ADDRSTRLEN);

    ks_printf("Socket Started Hosting (%s:%i)\n", str, (int)ntohs(self->sa_addr.sin_port));
    struct sockaddr_in clientaddr;
    socklen_t addrlen;

    while (true) {
        ks_GIL_unlock();
        int newfd = accept (self->sockfd, (struct sockaddr *) &clientaddr, &addrlen);
        ks_GIL_lock();

        if (newfd >= 0) {
            printf("NEW CONNECTION: %d\n", newfd);

        }
    }

    return KSO_NONE;
}




// now, export them all
static ks_module get_module() {
    
    ks_module mod = ks_module_new(MODULE_NAME);

    KS_INIT_TYPE_OBJ(sock_type_Socket, "sock.Socket");

    ks_type_set_cn(sock_type_Socket, (ks_dict_ent_c[]){
        {"__new__",             (ks_obj)ks_cfunc_new2(Socket_new_, "sock.Socket.__new__(self)")},
        {"__free__",            (ks_obj)ks_cfunc_new2(Socket_free_, "sock.Socket.__free__(self)")},

        {"connect",             (ks_obj)ks_cfunc_new2(Socket_connect_, "sock.Socket.connect(self, address, port)")},
        {"bind",                (ks_obj)ks_cfunc_new2(Socket_bind_, "sock.Socket.bind(self, address, port)")},
        
        {"listen",              (ks_obj)ks_cfunc_new2(Socket_listen_, "sock.Socket.listen(self, num)")},
        {"accept",              (ks_obj)ks_cfunc_new2(Socket_accept_, "sock.Socket.accept(self)")},

        {"send",                (ks_obj)ks_cfunc_new2(Socket_send_, "sock.Socket.send(self, msg)")},
        {"recv",                (ks_obj)ks_cfunc_new2(Socket_recv_, "sock.Socket.recv(self, sz)")},

        {"get_name",            (ks_obj)ks_cfunc_new2(Socket_get_name_, "sock.Socket.get_name(self)")},
        {"get_port",            (ks_obj)ks_cfunc_new2(Socket_get_port_, "sock.Socket.get_port(self)")},

        {"handle_forever",      (ks_obj)ks_cfunc_new2(Socket_handle_forever_, "sock.Socket.handle_forever(self)")},

        /*
        {"__getitem__",    (ks_obj)ks_cfunc_new2(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
        {"__setitem__",    (ks_obj)ks_cfunc_new2(array_setitem_, "nx.array.__setitem__(self, *idxs)")},

        {"shape",          (ks_obj)ks_cfunc_new2(array_shape_, "nx.array.shape(self)")},*/

        {NULL, NULL},
    });

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* constants */
        {"Socket",     (ks_obj)sock_type_Socket},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
