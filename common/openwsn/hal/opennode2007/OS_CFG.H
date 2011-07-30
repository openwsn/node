/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2001, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                  uC/OS-II Configuration File for V2.51
*
* File : OS_CFG.H
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define HAL_MAX_EVENTS             2    /* Max. number of event control blocks in your application ...  */
                                       /* ... MUST be > 0                                              */
#define HAL_MAX_FLAGS              5    /* Max. number of Event Flag Groups    in your application ...  */
                                       /* ... MUST be > 0                                              */
#define HAL_MAX_MEM_PART           5    /* Max. number of memory partitions ...                         */
                                       /* ... MUST be > 0                                              */
#define HAL_MAX_QS                 2    /* Max. number of queue control blocks in your application ...  */
                                       /* ... MUST be > 0                                              */
#define HAL_MAX_TASKS             11    /* Max. number of tasks in your application ...                 */
                                       /* ... MUST be >= 2                                             */

#define HAL_LOWEST_PRIO           12    /* Defines the lowest priority that can be assigned ...         */
                                       /* ... MUST NEVER be higher than 63!                            */

#define HAL_TASK_IDLE_STK_SIZE   512    /* Idle task stack size (# of OS_STK wide entries)              */

#define HAL_TASK_STAT_EN           1    /* Enable (1) or Disable(0) the statistics task                 */
#define HAL_TASK_STAT_STK_SIZE   512    /* Statistics task stack size (# of OS_STK wide entries)        */

#define HAL_ARG_CHK_EN             1    /* Enable (1) or Disable (0) argument checking                  */
#define HAL_CPU_HOOKS_EN           1    /* uC/OS-II hooks are found in the processor port files         */


                                       /* ----------------------- EVENT FLAGS ------------------------ */
#define HAL_FLAG_EN                1    /* Enable (1) or Disable (0) code generation for EVENT FLAGS    */
#define HAL_FLAG_WAIT_CLR_EN       1    /* Include code for Wait on Clear EVENT FLAGS                   */
#define HAL_FLAG_ACCEPT_EN         1    /*     Include code for OSFlagAccept()                          */
#define HAL_FLAG_DEL_EN            1    /*     Include code for OSFlagDel()                             */
#define HAL_FLAG_QUERY_EN          1    /*     Include code for OSFlagQuery()                           */


                                       /* -------------------- MESSAGE MAILBOXES --------------------- */
#define OS_MBOX_EN                1    /* Enable (1) or Disable (0) code generation for MAILBOXES      */
#define OS_MBOX_ACCEPT_EN         1    /*     Include code for OSMboxAccept()                          */
#define OS_MBOX_DEL_EN            1    /*     Include code for OSMboxDel()                             */
#define OS_MBOX_POST_EN           1    /*     Include code for OSMboxPost()                            */
#define OS_MBOX_POST_OPT_EN       1    /*     Include code for OSMboxPostOpt()                         */
#define OS_MBOX_QUERY_EN          1    /*     Include code for OSMboxQuery()                           */


                                       /* --------------------- MEMORY MANAGEMENT -------------------- */
#define OS_MEM_EN                 1    /* Enable (1) or Disable (0) code generation for MEMORY MANAGER */
#define OS_MEM_QUERY_EN           1    /*     Include code for OSMemQuery()                            */


                                       /* ---------------- MUTUAL EXCLUSION SEMAPHORES --------------- */
#define OS_MUTEX_EN               1    /* Enable (1) or Disable (0) code generation for MUTEX          */
#define OS_MUTEX_ACCEPT_EN        1    /*     Include code for OSMutexAccept()                         */
#define OS_MUTEX_DEL_EN           1    /*     Include code for OSMutexDel()                            */
#define OS_MUTEX_QUERY_EN         1    /*     Include code for OSMutexQuery()                          */


                                       /* ---------------------- MESSAGE QUEUES ---------------------- */
#define OS_Q_EN                   1    /* Enable (1) or Disable (0) code generation for QUEUES         */
#define OS_Q_ACCEPT_EN            1    /*     Include code for OSQAccept()                             */
#define OS_Q_DEL_EN               1    /*     Include code for OSQDel()                                */
#define OS_Q_FLUSH_EN             1    /*     Include code for OSQFlush()                              */
#define OS_Q_POST_EN              1    /*     Include code for OSQPost()                               */
#define OS_Q_POST_FRONT_EN        1    /*     Include code for OSQPostFront()                          */
#define OS_Q_POST_OPT_EN          1    /*     Include code for OSQPostOpt()                            */
#define OS_Q_QUERY_EN             1    /*     Include code for OSQQuery()                              */


                                       /* ------------------------ SEMAPHORES ------------------------ */
#define OS_SEM_EN                 1    /* Enable (1) or Disable (0) code generation for SEMAPHORES     */
#define OS_SEM_ACCEPT_EN          1    /*    Include code for OSSemAccept()                            */
#define OS_SEM_DEL_EN             1    /*    Include code for OSSemDel()                               */
#define OS_SEM_QUERY_EN           1    /*    Include code for OSSemQuery()                             */


                                       /* --------------------- TASK MANAGEMENT ---------------------- */
#define HAL_TASK_CHANGE_PRIO_EN    1    /*     Include code for OSTaskChangePrio()                      */
#define HAL_TASK_CREATE_EN         1    /*     Include code for OSTaskCreate()                          */
#define HAL_TASK_CREATE_EXT_EN     1    /*     Include code for OSTaskCreateExt()                       */
#define HAL_TASK_DEL_EN            1    /*     Include code for OSTaskDel()                             */
#define HAL_TASK_SUSPEND_EN        1    /*     Include code for OSTaskSuspend() and OSTaskResume()      */
#define HAL_TASK_QUERY_EN          1    /*     Include code for OSTaskQuery()                           */


                                       /* --------------------- TIME MANAGEMENT ---------------------- */
#define OS_TIME_DLY_HMSM_EN       1    /*     Include code for OSTimeDlyHMSM()                         */
#define OS_TIME_DLY_RESUME_EN     1    /*     Include code for OSTimeDlyResume()                       */
#define OS_TIME_GET_SET_EN        1    /*     Include code for OSTimeGet() and OSTimeSet()             */


                                       /* ---------------------- MISCELLANEOUS ----------------------- */
#define HAL_sched_LOCK_EN          1    /*     Include code for OSSchedLock() and OSSchedUnlock()       */


#define HAL_TICKS_PER_SEC        200    /* Set the number of ticks in one second                        */


//typedef INT16U             HAL_FLAGS;   /* Date type for event flag bits (8, 16 or 32 bits)             */
