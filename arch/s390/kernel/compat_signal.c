/*
 *  arch/s390/kernel/compat_signal.c
 *
 *    Copyright (C) IBM Corp. 2000,2006
 *    Author(s): Denis Joseph Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
 *               Gerhard Tonn (ton@de.ibm.com)                  
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  1997-11-28  Modified for POSIX.1b signals by Richard Henderson
 */

#include <linux/compat.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/tty.h>
#include <linux/personality.h>
#include <linux/binfmts.h>
#include <asm/ucontext.h>
#include <asm/uaccess.h>
#include <asm/lowcore.h>
#include <asm/switch_to.h>
#include "compat_linux.h"
#include "compat_ptrace.h"
#include "entry.h"

#define _BLOCKABLE (~(sigmask(SIGKILL) | sigmask(SIGSTOP)))

typedef struct 
{
	__u8 callee_used_stack[__SIGNAL_FRAMESIZE32];
	struct sigcontext32 sc;
	_sigregs32 sregs;
	int signo;
	__u32 gprs_high[NUM_GPRS];
	__u8 retcode[S390_SYSCALL_SIZE];
} sigframe32;

typedef struct 
{
	__u8 callee_used_stack[__SIGNAL_FRAMESIZE32];
	__u8 retcode[S390_SYSCALL_SIZE];
	compat_siginfo_t info;
	struct ucontext32 uc;
	__u32 gprs_high[NUM_GPRS];
} rt_sigframe32;

int copy_siginfo_to_user32(compat_siginfo_t __user *to, siginfo_t *from)
{
	int err;

	if (!access_ok (VERIFY_WRITE, to, sizeof(compat_siginfo_t)))
		return -EFAULT;

	err = __put_user(from->si_signo, &to->si_signo);
	err |= __put_user(from->si_errno, &to->si_errno);
	err |= __put_user((short)from->si_code, &to->si_code);
	if (from->si_code < 0)
		err |= __copy_to_user(&to->_sifields._pad, &from->_sifields._pad, SI_PAD_SIZE);
	else {
		switch (from->si_code >> 16) {
		case __SI_RT >> 16: 
		case __SI_MESGQ >> 16:
			err |= __put_user(from->si_int, &to->si_int);
			
		case __SI_KILL >> 16:
			err |= __put_user(from->si_pid, &to->si_pid);
			err |= __put_user(from->si_uid, &to->si_uid);
			break;
		case __SI_CHLD >> 16:
			err |= __put_user(from->si_pid, &to->si_pid);
			err |= __put_user(from->si_uid, &to->si_uid);
			err |= __put_user(from->si_utime, &to->si_utime);
			err |= __put_user(from->si_stime, &to->si_stime);
			err |= __put_user(from->si_status, &to->si_status);
			break;
		case __SI_FAULT >> 16:
			err |= __put_user((unsigned long) from->si_addr,
					  &to->si_addr);
			break;
		case __SI_POLL >> 16:
			err |= __put_user(from->si_band, &to->si_band);
			err |= __put_user(from->si_fd, &to->si_fd);
			break;
		case __SI_TIMER >> 16:
			err |= __put_user(from->si_tid, &to->si_tid);
			err |= __put_user(from->si_overrun, &to->si_overrun);
			err |= __put_user(from->si_int, &to->si_int);
			break;
		default:
			break;
		}
	}
	return err;
}

int copy_siginfo_from_user32(siginfo_t *to, compat_siginfo_t __user *from)
{
	int err;
	u32 tmp;

	if (!access_ok (VERIFY_READ, from, sizeof(compat_siginfo_t)))
		return -EFAULT;

	err = __get_user(to->si_signo, &from->si_signo);
	err |= __get_user(to->si_errno, &from->si_errno);
	err |= __get_user(to->si_code, &from->si_code);

	if (to->si_code < 0)
		err |= __copy_from_user(&to->_sifields._pad, &from->_sifields._pad, SI_PAD_SIZE);
	else {
		switch (to->si_code >> 16) {
		case __SI_RT >> 16: 
		case __SI_MESGQ >> 16:
			err |= __get_user(to->si_int, &from->si_int);
			
		case __SI_KILL >> 16:
			err |= __get_user(to->si_pid, &from->si_pid);
			err |= __get_user(to->si_uid, &from->si_uid);
			break;
		case __SI_CHLD >> 16:
			err |= __get_user(to->si_pid, &from->si_pid);
			err |= __get_user(to->si_uid, &from->si_uid);
			err |= __get_user(to->si_utime, &from->si_utime);
			err |= __get_user(to->si_stime, &from->si_stime);
			err |= __get_user(to->si_status, &from->si_status);
			break;
		case __SI_FAULT >> 16:
			err |= __get_user(tmp, &from->si_addr);
			to->si_addr = (void __force __user *)
				(u64) (tmp & PSW32_ADDR_INSN);
			break;
		case __SI_POLL >> 16:
			err |= __get_user(to->si_band, &from->si_band);
			err |= __get_user(to->si_fd, &from->si_fd);
			break;
		case __SI_TIMER >> 16:
			err |= __get_user(to->si_tid, &from->si_tid);
			err |= __get_user(to->si_overrun, &from->si_overrun);
			err |= __get_user(to->si_int, &from->si_int);
			break;
		default:
			break;
		}
	}
	return err;
}

asmlinkage long
sys32_sigaction(int sig, const struct old_sigaction32 __user *act,
		 struct old_sigaction32 __user *oact)
{
        struct k_sigaction new_ka, old_ka;
	unsigned long sa_handler, sa_restorer;
        int ret;

        if (act) {
		compat_old_sigset_t mask;
		if (!access_ok(VERIFY_READ, act, sizeof(*act)) ||
		    __get_user(sa_handler, &act->sa_handler) ||
		    __get_user(sa_restorer, &act->sa_restorer) ||
		    __get_user(new_ka.sa.sa_flags, &act->sa_flags) ||
		    __get_user(mask, &act->sa_mask))
			return -EFAULT;
		new_ka.sa.sa_handler = (__sighandler_t) sa_handler;
		new_ka.sa.sa_restorer = (void (*)(void)) sa_restorer;
		siginitset(&new_ka.sa.sa_mask, mask);
        }

        ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);

	if (!ret && oact) {
		sa_handler = (unsigned long) old_ka.sa.sa_handler;
		sa_restorer = (unsigned long) old_ka.sa.sa_restorer;
		if (!access_ok(VERIFY_WRITE, oact, sizeof(*oact)) ||
		    __put_user(sa_handler, &oact->sa_handler) ||
		    __put_user(sa_restorer, &oact->sa_restorer) ||
		    __put_user(old_ka.sa.sa_flags, &oact->sa_flags) ||
		    __put_user(old_ka.sa.sa_mask.sig[0], &oact->sa_mask))
			return -EFAULT;
        }

	return ret;
}

asmlinkage long
sys32_rt_sigaction(int sig, const struct sigaction32 __user *act,
	   struct sigaction32 __user *oact,  size_t sigsetsize)
{
	struct k_sigaction new_ka, old_ka;
	unsigned long sa_handler;
	int ret;
	compat_sigset_t set32;

	
	if (sigsetsize != sizeof(compat_sigset_t))
		return -EINVAL;

	if (act) {
		ret = get_user(sa_handler, &act->sa_handler);
		ret |= __copy_from_user(&set32, &act->sa_mask,
					sizeof(compat_sigset_t));
		new_ka.sa.sa_mask.sig[0] =
			set32.sig[0] | (((long)set32.sig[1]) << 32);
		ret |= __get_user(new_ka.sa.sa_flags, &act->sa_flags);
		
		if (ret)
			return -EFAULT;
		new_ka.sa.sa_handler = (__sighandler_t) sa_handler;
	}

	ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);

	if (!ret && oact) {
		set32.sig[1] = (old_ka.sa.sa_mask.sig[0] >> 32);
		set32.sig[0] = old_ka.sa.sa_mask.sig[0];
		ret = put_user((unsigned long)old_ka.sa.sa_handler, &oact->sa_handler);
		ret |= __copy_to_user(&oact->sa_mask, &set32,
				      sizeof(compat_sigset_t));
		ret |= __put_user(old_ka.sa.sa_flags, &oact->sa_flags);
	}

	return ret;
}

asmlinkage long
sys32_sigaltstack(const stack_t32 __user *uss, stack_t32 __user *uoss)
{
	struct pt_regs *regs = task_pt_regs(current);
	stack_t kss, koss;
	unsigned long ss_sp;
	int ret, err = 0;
	mm_segment_t old_fs = get_fs();

	if (uss) {
		if (!access_ok(VERIFY_READ, uss, sizeof(*uss)))
			return -EFAULT;
		err |= __get_user(ss_sp, &uss->ss_sp);
		err |= __get_user(kss.ss_size, &uss->ss_size);
		err |= __get_user(kss.ss_flags, &uss->ss_flags);
		if (err)
			return -EFAULT;
		kss.ss_sp = (void __user *) ss_sp;
	}

	set_fs (KERNEL_DS);
	ret = do_sigaltstack((stack_t __force __user *) (uss ? &kss : NULL),
			     (stack_t __force __user *) (uoss ? &koss : NULL),
			     regs->gprs[15]);
	set_fs (old_fs);

	if (!ret && uoss) {
		if (!access_ok(VERIFY_WRITE, uoss, sizeof(*uoss)))
			return -EFAULT;
		ss_sp = (unsigned long) koss.ss_sp;
		err |= __put_user(ss_sp, &uoss->ss_sp);
		err |= __put_user(koss.ss_size, &uoss->ss_size);
		err |= __put_user(koss.ss_flags, &uoss->ss_flags);
		if (err)
			return -EFAULT;
	}
	return ret;
}

static int save_sigregs32(struct pt_regs *regs, _sigregs32 __user *sregs)
{
	_s390_regs_common32 regs32;
	int err, i;

	regs32.psw.mask = psw32_user_bits |
		((__u32)(regs->psw.mask >> 32) & PSW32_MASK_USER);
	regs32.psw.addr = (__u32) regs->psw.addr |
		(__u32)(regs->psw.mask & PSW_MASK_BA);
	for (i = 0; i < NUM_GPRS; i++)
		regs32.gprs[i] = (__u32) regs->gprs[i];
	save_access_regs(current->thread.acrs);
	memcpy(regs32.acrs, current->thread.acrs, sizeof(regs32.acrs));
	err = __copy_to_user(&sregs->regs, &regs32, sizeof(regs32));
	if (err)
		return err;
	save_fp_regs(&current->thread.fp_regs);
	
	return __copy_to_user(&sregs->fpregs, &current->thread.fp_regs,
			      sizeof(_s390_fp_regs32));
}

static int restore_sigregs32(struct pt_regs *regs,_sigregs32 __user *sregs)
{
	_s390_regs_common32 regs32;
	int err, i;

	
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	err = __copy_from_user(&regs32, &sregs->regs, sizeof(regs32));
	if (err)
		return err;
	regs->psw.mask = (regs->psw.mask & ~PSW_MASK_USER) |
		(__u64)(regs32.psw.mask & PSW32_MASK_USER) << 32 |
		(__u64)(regs32.psw.addr & PSW32_ADDR_AMODE);
	regs->psw.addr = (__u64)(regs32.psw.addr & PSW32_ADDR_INSN);
	for (i = 0; i < NUM_GPRS; i++)
		regs->gprs[i] = (__u64) regs32.gprs[i];
	memcpy(current->thread.acrs, regs32.acrs, sizeof(current->thread.acrs));
	restore_access_regs(current->thread.acrs);

	err = __copy_from_user(&current->thread.fp_regs, &sregs->fpregs,
			       sizeof(_s390_fp_regs32));
	current->thread.fp_regs.fpc &= FPC_VALID_MASK;
	if (err)
		return err;

	restore_fp_regs(&current->thread.fp_regs);
	clear_thread_flag(TIF_SYSCALL);	
	return 0;
}

static int save_sigregs_gprs_high(struct pt_regs *regs, __u32 __user *uregs)
{
	__u32 gprs_high[NUM_GPRS];
	int i;

	for (i = 0; i < NUM_GPRS; i++)
		gprs_high[i] = regs->gprs[i] >> 32;

	return __copy_to_user(uregs, &gprs_high, sizeof(gprs_high));
}

static int restore_sigregs_gprs_high(struct pt_regs *regs, __u32 __user *uregs)
{
	__u32 gprs_high[NUM_GPRS];
	int err, i;

	err = __copy_from_user(&gprs_high, uregs, sizeof(gprs_high));
	if (err)
		return err;
	for (i = 0; i < NUM_GPRS; i++)
		*(__u32 *)&regs->gprs[i] = gprs_high[i];
	return 0;
}

asmlinkage long sys32_sigreturn(void)
{
	struct pt_regs *regs = task_pt_regs(current);
	sigframe32 __user *frame = (sigframe32 __user *)regs->gprs[15];
	sigset_t set;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set.sig, &frame->sc.oldmask, _SIGMASK_COPY_SIZE32))
		goto badframe;
	sigdelsetmask(&set, ~_BLOCKABLE);
	set_current_blocked(&set);
	if (restore_sigregs32(regs, &frame->sregs))
		goto badframe;
	if (restore_sigregs_gprs_high(regs, frame->gprs_high))
		goto badframe;
	return regs->gprs[2];
badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

asmlinkage long sys32_rt_sigreturn(void)
{
	struct pt_regs *regs = task_pt_regs(current);
	rt_sigframe32 __user *frame = (rt_sigframe32 __user *)regs->gprs[15];
	sigset_t set;
	stack_t st;
	__u32 ss_sp;
	int err;
	mm_segment_t old_fs = get_fs();

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;
	sigdelsetmask(&set, ~_BLOCKABLE);
	set_current_blocked(&set);
	if (restore_sigregs32(regs, &frame->uc.uc_mcontext))
		goto badframe;
	if (restore_sigregs_gprs_high(regs, frame->gprs_high))
		goto badframe;
	err = __get_user(ss_sp, &frame->uc.uc_stack.ss_sp);
	st.ss_sp = compat_ptr(ss_sp);
	err |= __get_user(st.ss_size, &frame->uc.uc_stack.ss_size);
	err |= __get_user(st.ss_flags, &frame->uc.uc_stack.ss_flags);
	if (err)
		goto badframe; 
	set_fs (KERNEL_DS);
	do_sigaltstack((stack_t __force __user *)&st, NULL, regs->gprs[15]);
	set_fs (old_fs);
	return regs->gprs[2];
badframe:
	force_sig(SIGSEGV, current);
	return 0;
}	



static inline void __user *
get_sigframe(struct k_sigaction *ka, struct pt_regs * regs, size_t frame_size)
{
	unsigned long sp;

	
	sp = (unsigned long) A(regs->gprs[15]);

	
	if (on_sig_stack(sp) && !on_sig_stack((sp - frame_size) & -8UL))
		return (void __user *) -1UL;

	
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (! sas_ss_flags(sp))
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	
	else if (!user_mode(regs) &&
		 !(ka->sa.sa_flags & SA_RESTORER) &&
		 ka->sa.sa_restorer) {
		sp = (unsigned long) ka->sa.sa_restorer;
	}

	return (void __user *)((sp - frame_size) & -8ul);
}

static inline int map_signal(int sig)
{
	if (current_thread_info()->exec_domain
	    && current_thread_info()->exec_domain->signal_invmap
	    && sig < 32)
		return current_thread_info()->exec_domain->signal_invmap[sig];
        else
		return sig;
}

static int setup_frame32(int sig, struct k_sigaction *ka,
			sigset_t *set, struct pt_regs * regs)
{
	sigframe32 __user *frame = get_sigframe(ka, regs, sizeof(sigframe32));
	if (!access_ok(VERIFY_WRITE, frame, sizeof(sigframe32)))
		goto give_sigsegv;

	if (frame == (void __user *) -1UL)
		goto give_sigsegv;

	if (__copy_to_user(&frame->sc.oldmask, &set->sig, _SIGMASK_COPY_SIZE32))
		goto give_sigsegv;

	if (save_sigregs32(regs, &frame->sregs))
		goto give_sigsegv;
	if (save_sigregs_gprs_high(regs, frame->gprs_high))
		goto give_sigsegv;
	if (__put_user((unsigned long) &frame->sregs, &frame->sc.sregs))
		goto give_sigsegv;

	if (ka->sa.sa_flags & SA_RESTORER) {
		regs->gprs[14] = (__u64) ka->sa.sa_restorer | PSW32_ADDR_AMODE;
	} else {
		regs->gprs[14] = (__u64) frame->retcode | PSW32_ADDR_AMODE;
		if (__put_user(S390_SYSCALL_OPCODE | __NR_sigreturn,
			       (u16 __force __user *)(frame->retcode)))
			goto give_sigsegv;
        }

	
	if (__put_user(regs->gprs[15], (unsigned int __user *) frame))
		goto give_sigsegv;

	
	regs->gprs[15] = (__force __u64) frame;
	regs->psw.mask |= PSW_MASK_BA;		
	regs->psw.addr = (__force __u64) ka->sa.sa_handler;

	regs->gprs[2] = map_signal(sig);
	regs->gprs[3] = (__force __u64) &frame->sc;

	if (sig == SIGSEGV || sig == SIGBUS || sig == SIGILL ||
	    sig == SIGTRAP || sig == SIGFPE) {
		
		regs->gprs[4] = regs->int_code & 127;
		regs->gprs[5] = regs->int_parm_long;
	}

	
	if (__put_user(regs->gprs[2], (int __force __user *) &frame->signo))
		goto give_sigsegv;
	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

static int setup_rt_frame32(int sig, struct k_sigaction *ka, siginfo_t *info,
			   sigset_t *set, struct pt_regs * regs)
{
	int err = 0;
	rt_sigframe32 __user *frame = get_sigframe(ka, regs, sizeof(rt_sigframe32));
	if (!access_ok(VERIFY_WRITE, frame, sizeof(rt_sigframe32)))
		goto give_sigsegv;

	if (frame == (void __user *) -1UL)
		goto give_sigsegv;

	if (copy_siginfo_to_user32(&frame->info, info))
		goto give_sigsegv;

	
	err |= __put_user(UC_EXTENDED, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __put_user(current->sas_ss_sp, &frame->uc.uc_stack.ss_sp);
	err |= __put_user(sas_ss_flags(regs->gprs[15]),
	                  &frame->uc.uc_stack.ss_flags);
	err |= __put_user(current->sas_ss_size, &frame->uc.uc_stack.ss_size);
	err |= save_sigregs32(regs, &frame->uc.uc_mcontext);
	err |= save_sigregs_gprs_high(regs, frame->gprs_high);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));
	if (err)
		goto give_sigsegv;

	if (ka->sa.sa_flags & SA_RESTORER) {
		regs->gprs[14] = (__u64) ka->sa.sa_restorer | PSW32_ADDR_AMODE;
	} else {
		regs->gprs[14] = (__u64) frame->retcode | PSW32_ADDR_AMODE;
		err |= __put_user(S390_SYSCALL_OPCODE | __NR_rt_sigreturn,
				  (u16 __force __user *)(frame->retcode));
	}

	
	if (__put_user(regs->gprs[15], (unsigned int __force __user *) frame))
		goto give_sigsegv;

	
	regs->gprs[15] = (__force __u64) frame;
	regs->psw.mask |= PSW_MASK_BA;		
	regs->psw.addr = (__u64) ka->sa.sa_handler;

	regs->gprs[2] = map_signal(sig);
	regs->gprs[3] = (__force __u64) &frame->info;
	regs->gprs[4] = (__force __u64) &frame->uc;
	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

	

int handle_signal32(unsigned long sig, struct k_sigaction *ka,
		    siginfo_t *info, sigset_t *oldset, struct pt_regs *regs)
{
	int ret;

	
	if (ka->sa.sa_flags & SA_SIGINFO)
		ret = setup_rt_frame32(sig, ka, info, oldset, regs);
	else
		ret = setup_frame32(sig, ka, oldset, regs);
	if (ret)
		return ret;
	block_sigmask(ka, sig);
	return 0;
}

