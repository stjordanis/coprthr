/* xcmdsched.c 
 *
 * Copyright (c) 2009-2013 Brown Deer Technology, LLC.  All Rights Reserved.
 *
 * This software was developed by Brown Deer Technology, LLC.
 * For more information contact info@browndeertechnology.com
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3 (LGPLv3)
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* DAR */

#define _GNU_SOURCE
#include <sched.h>
#include <sys/time.h>
#include <pthread.h>

#include <CL/cl.h>
#include <assert.h>

#include "printcl.h"
#include "cpuset_type.h"
#include "cmdsched.h"
#include "command_queue.h"
#include "cmdcall.h"
#include "event.h"


#define __CL_COMMAND_OFFSET (CL_COMMAND_NDRANGE_KERNEL-1)

void* cmdqx1( void* argp )
{

	struct timeval tv;

//	cl_command_queue cmdq = (cl_command_queue)argp;
	struct coprthr_device* dev = (struct coprthr_device*)argp;
	struct coprthr_command_queue* cmdq1 = dev->devstate->cmdq;

//	cl_device_id devid = cmdq->devid;


	/* XXX set cpu affinity of td based on device recommendation -DAR */
	
	pthread_setaffinity_np(pthread_self(),sizeof(cpu_set_t),
//		&__resolve_devid(devid,cpumask));
//		&__resolve_devid_devstate(devid,cpumask));
		&(dev->devstate->cpumask));


//	printcl( CL_DEBUG "cmdqx0: cmdq=%p devid=%p cpumask_count=%d",
//		cmdq,devid,CPU_COUNT(&__resolve_devid(devid,cpumask)));
	printcl( CL_DEBUG "cmdqx1: cmdq1=%p dev=%p cpumask_count=?",
		dev->devstate->cmdq,dev);
/* XXX most of the useful cpu_set macros are missing from rhel/centos 
 * maybe they will get to that after they upgrade to a modern GCC version -DAR
 */


//	cmdcall_t* cmdcall = __resolve_devid(cmdq->devid,v_cmdcall);
//	cmdcall_t* cmdcall = __resolve_devid_devops(cmdq->devid,v_cmdcall);
	cmdcall_t* cmdcall = dev->devops->v_cmdcall;

	assert(CL_COMMAND_RELEASE_GL_OBJECTS - CLCMD_OFFSET == CLCMD_NUM);

	cl_event ev;

	__lock_cmdq1(cmdq1);

	printcl( CL_DEBUG "cmdqx1: idle");

	while (cmdq1->qstat == 0) {
		printcl( CL_DEBUG "idle-sleep\n");
		__wait_cmdq1(cmdq1);
		printcl( CL_DEBUG "idle-wake\n");
	}

	__unlock_cmdq1(cmdq1);


	__lock_cmdq1(cmdq1);

	printcl( CL_INFO "cmdqx1: run");

	while (cmdq1->qstat == 1) {

		printcl( CL_DEBUG "cmdqx1:" " queued %p",cmdq1->cmds_queued.tqh_first);

		while (ev = cmdq1->cmds_queued.tqh_first) {


			printcl( CL_DEBUG "cmdqx1:" " submit cmd ev %p",ev);

			/* 
			 * submit cmd 
			 */

			printcl( CL_DEBUG "cmdqx1: attempt __lock_event");
			__lock_event(ev);

			printcl( CL_DEBUG "cmdqx1: attempt __retain_event");
			__retain_event(ev);

			TAILQ_REMOVE(&(cmdq1->cmds_queued),ev,imp.cmds);
			cmdq1->cmd_submitted = ev;
			ev->cmd_stat = CL_SUBMITTED;
			gettimeofday(&tv,0);
			ev->tm_submit = tv.tv_sec * 1000000000 + tv.tv_usec * 1000;
			ev->tm_start = tv.tv_sec * 1000000000 + tv.tv_usec * 1000;

			__unlock_cmdq1(cmdq1);
			printcl( CL_DEBUG "%p: submitted %x\n",ev,ev->cmd);
//			cmdcall[ev->cmd-CLCMD_OFFSET](ev->imp.cmd_argp);
//			cmdcall[ev->cmd-CLCMD_OFFSET](devid,ev->imp.cmd_argp);
			cmdcall[ev->cmd-CLCMD_OFFSET](dev,ev->imp.cmd_argp);
			__lock_cmdq1(cmdq1);

			__sig_event(ev);

			__unlock_event(ev);


			/* XXX cmdcall should either be moved down to running, OR the call
			 * XXX should be executed via fork so that event can be marked running
			 * XXX when it is really running.  issue is confusion over submit/run
			 * XXX which i guess means that a cmd can be submitted and held for
			 * XXX execution in a derivative device queue. -DAR */

			/* 
			 * cmd running 
			 */

			__lock_event(ev);

			ev = cmdq1->cmd_submitted;
			cmdq1->cmd_submitted = 0;
			cmdq1->cmd_running = ev;
			ev->cmd_stat = CL_RUNNING;

			__unlock_cmdq1(cmdq1);
			printcl( CL_DEBUG "%p: running %x\n",ev,ev->cmd);
			__lock_cmdq1(cmdq1);

			__sig_event(ev);

			__unlock_event(ev);



			/* 
			 * cmd complete 
			 */

			__lock_event(ev);

			ev = cmdq1->cmd_running;
			cmdq1->cmd_running = 0;
			TAILQ_INSERT_TAIL(&(cmdq1->cmds_complete),ev,imp.cmds);
//			cmdq->imp.cmd_running = ev;
			ev->cmd_stat = CL_COMPLETE;
			gettimeofday(&tv,0);
			ev->tm_end = tv.tv_sec * 1000000000 + tv.tv_usec * 1000;

			__sig_event(ev);

			__release_event(ev);
			/* unlock implicit in release -DAR */

			printcl( CL_DEBUG "%p: complete %x\n",ev,ev->cmd);
			
		}

		/* XXX added to wake up thread that might be blocking on finish(cmdq) */
		__sig_cmdq1(cmdq1);

		printcl( CL_DEBUG "run-cmdqx1 sleep\n");
		__wait_cmdq1(cmdq1);
		printcl( CL_DEBUG "run-cmdqx1 wake\n");

	}

	__unlock_cmdq1(cmdq1);
	
	printcl( CL_INFO "cmdqx1: shutdown");
	
	pthread_exit((void*)0);
}
