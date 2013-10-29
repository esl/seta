//
//  seta.h
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include <stdbool.h>

//! Handle for spawn next.
/*! Handle used for declaring a new spawn next, setting up all the related continuations
	and finally committing it.*/
typedef struct {
	void *closure;
} seta_handle_spawn_next_t;

//! Continuation
typedef struct {
	void *closure;
	void *arg;
	// info
	int n_arg;
} seta_cont_t;


//! Context needed by seta for the data exchange between the scheduler and threads.
typedef struct {
	int level;
	int n_local_proc;
	bool is_last_thread;
	bool spawn_next_done;
	bool free_args_done;
	// info
	char *spawned;
	int args_size;
	void *arg_name_list;
	int allocated_ancients;
	int closure_id;
	void *allocated_ancient_list;
	void *spawn_list;
	void *args;
} seta_context_t;

//! List of argument name
typedef void *seta_arg_name_list_t;




/*! \fn seta_cont_t seta_create_cont(void *arg, seta_handle_spawn_next_t hsn)
 \brief Create a new continuation to the specified argument. According to the theory, 
 this continuation is linked to the closure of the thread that is going to be spawned and 
 the closure will increase its join counter by a unit.
 \param arg Pointer to an argument of the thread is being spawned.
 \param hsn Handle to the thread is being spawned.
 \return New continuation.
*/
seta_cont_t seta_create_cont(void *, seta_handle_spawn_next_t);

/*! \fn int seta_start(void *fun, int n)
	\brief Starts the scheduler.
	\param fun the entry thread.
	\param n the multiplicity of the scheduler.
	\return 0 if the scheduler is correctly started, -1 if some error occurred.
*/
int seta_start(void *, int);

/*! \fn void * seta_alloc_args(long size)
	\brief Allocates the indicated amount of memory space in the heap which will be used 
		by the user as a space where storing all the arguments to provide 
		to the next new thread will be spawned.
	\param size the amount expressed in byte.
	\return Pointer to the allocated memory
*/
void * seta_alloc_args(long);

/*! \fn void seta_free_args(seta_context_t *context)
	\brief Free the allocated space retained for storing the arguments of the current thread.
		After that the field args of the context will not more accessible. Thus all the needed
		arguments have to be saved in local variables before.
	\param context of the current thread.
*/
void seta_free_args(seta_context_t *);

/*! \fn seta_handle_spawn_next_t seta_prepare_spawn_next(void *fun, void *args, seta_context_t *context)
	\brief Prepares the new thread to be spawned from the current thread.
	\param fun Pointer to the function that will be mapped to the new thread.
	\param args Memory space containing the arguments for the new spawned thread.
	\param context Context of the current thread.
	\return Handle to the new thread is going to be spawned.
 */
seta_handle_spawn_next_t seta_prepare_spawn_next(void *, void *, seta_context_t *);


/*! \fn void seta_spawn_next(seta_handle_spawn_next_t hsn)
	\brief It actually spawns the thread, which means that after that a new thread will be 
		ready to be executed if the thread is complete (all his arguments are set), otherwise
		it stalls waiting for other threads to send the necessary arguments to it.
	\param hsn The handle to the new thread is going to be spawned.
*/
void seta_spawn_next(seta_handle_spawn_next_t);

/*! \fn void seta_spawn(void *fun, void *args, seta_context_t *context)
	\brief Spawns a new thread. It will be at a certain time executed by the the processor which
		is executing the current thread or by another processor will possibly steal it.
	\param fun Pointer to the function that will be mapped to the new thread.
	\param args Memory space containing the arguments for the new spawned thread.
	\param context Context of the current thread.
*/
void seta_spawn(void *, void *, seta_context_t *);

/*! \fn void seta_send_argument(seta_cont_t cont, void *src, int size, seta_context_t *context)
	\brief Send an argument to the thread pointed by the continuation.
	\param cont Continuation pointing to the thread's argument to fill.
	\param src Pointer to the local variable to send.
	\param size local variable size.
	\param context Context of the current thread.
*/
void seta_send_argument(seta_cont_t, void *, int, seta_context_t *);

/*! \fn void seta_enable_info()
	\brief Enables the tracking of memory allocation and the evaluation of S1
*/
void seta_enable_info();

/*! \fn void seta_enable_graph(char *filename)
	\brief Enables the creation of a graph in dot format
	\param filename Filename of the dot file created
*/
void seta_enable_graph(char *);

/*! \fn seta_arg_name_list_t seta_arg_name_list_new()
	\brief Returns a list of argument name. After, it will be filled and set to the field 
		arg_name_list of the context. Seta will use it to assign labels to the nodes in the
		generated graph
	\return Argument name list
*/
seta_arg_name_list_t seta_arg_name_list_new();

/*! \fn void seta_arg_name_list_add(seta_arg_name_list_t ptr, const char *format, ...)
	\brief Adds a name to the argument name list.
	\param ptr Argument name list
	\param format Argument name containing optionally embedded format specifiers
	\param ... additional arguments
*/
void seta_arg_name_list_add(seta_arg_name_list_t, const char *, ...);

/*! \fn void seta_send_arg_name(seta_cont_t cont, const char *format, ...)
	\brief Sends an argument name to the thread pointed by the continuation. It is the analogous
		of send_message, but only needed for the creation of the graph.
	\param cont Continuation pointing to the thread's argument name to fill.
	\param format Argument name containing optionally embedded format specifiers
	\param ... additional arguments
*/
void seta_send_arg_name(seta_cont_t, const char *, ...);

/*! \fn void * seta_get_args(seta_context_t *context)
	\brief Returns the arguments for the current thread, which are the arguments provided by the
		thread's parent during the spawn plus the arguments sent by other threads subsequently
		filling any argument that was unknown at the time of the spawn.
	\param context Context of the current thread.
	\return Pointer to the argument space for the current thread.
*/
void * seta_get_args(seta_context_t *);

