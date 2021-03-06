#ifndef    _ARERR_H_
#define    _ARERR_H_

#define    SYS_ERROR_BASE   -9000
#define	   IPC_ERRBASE	(SYS_ERROR_BASE-100)

#define    SHM_CREATE_SHM_ERR (IPC_ERRBASE-1)
#define    SHM_ATTACH_SHM_ERR (IPC_ERRBASE-2)
#define    SHM_OPEN_SHM_ERR (IPC_ERRBASE-3)    
#define    SHM_DETACH_SHM_ERR (IPC_ERRBASE-4)
#define    SHM_REMOVE_SHM_ERR (IPC_ERRBASE-5)
#define    SEM_CREATE_SEM_ERR	(IPC_ERRBASE-6)
#define    SEM_INIT_SEM_ERR	(IPC_ERRBASE-7)
#define    SEM_OPEN_SEM_ERR	(IPC_ERRBASE-8)
#define    SEM_REMOVE_SEM_ERR	(IPC_ERRBASE-9)
#define    SEM_OPERATE_SEM_ERR	(IPC_ERRBASE-10)
#define    SEM_CONTROL_SEM_ERR	(IPC_ERRBASE-11)

#define	   LOG_ERRBASE	(SYS_ERROR_BASE-120)
#define    TRC_OPEN_TRACE_FILE_ERR  (LOG_ERRBASE-1)
#define    TRC_SEEK_TRACE_FILE_ERR  (LOG_ERRBASE-2)
#define    TRC_RENAME_TRACE_FILE_ERR  (LOG_ERRBASE-3)

#endif  
/*
** end of file arerr.h
*/
