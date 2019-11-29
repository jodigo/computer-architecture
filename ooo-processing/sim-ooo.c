/*
 * Tor M. Aamodt (aamodt@ece.ubc.ca) - Sept. 25, 2006
 * and other CPEN 411 instructors
 *
 * Based upon sim-safe.c from:
 *
 * SimpleScalar(TM) Tool Suite
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 * All Rights Reserved. 
 * 
 * THIS IS A LEGAL DOCUMENT, BY USING SIMPLESCALAR,
 * YOU ARE AGREEING TO THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted
 * as described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express
 * or implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship
 * purposes provided that this notice in its entirety accompanies all copies.
 * Copies of the modified software can be delivered to persons who use it
 * solely for nonprofit, educational, noncommercial research, and
 * noncommercial scholarship purposes provided that this notice in its
 * entirety accompanies all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a
 * copy of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "options.h"
#include "stats.h"
#include "sim.h"
#include "resource.h"

/*
 * This file implements a functional simulator.  This functional simulator is
 * the simplest, most user-friendly simulator in the simplescalar tool set.
 * Unlike sim-fast, this functional simulator checks for all instruction
 * errors, and the implementation is crafted for clarity rather than speed.
 */

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* track number of refs */
static counter_t sim_num_refs = 0;

/* maximum number of inst's to execute */
static unsigned int max_insts;

/* cycle counter */
unsigned sim_cycle;

/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-ooo: This simulator implements a functional simulator.  This\n"
"functional simulator is the simplest, most user-friendly simulator in the\n"
"simplescalar tool set.  Unlike sim-fast, this functional simulator checks\n"
"for all instruction errors, and the implementation is crafted for clarity\n"
"rather than speed.\n"
         );

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
           &max_insts, /* default */0,
           /* print */TRUE, /* format */NULL);

}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  /* nada */
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
           "total number of instructions executed",
           &sim_num_insn, sim_num_insn, NULL);

  stat_reg_uint(sdb, "sim_cycles",
           "total number of cycles",
           &sim_cycle, 0, NULL);
  stat_reg_formula(sdb, "sim_cpi",
           "cycles per instruction (CPI)",
           "sim_cycles / sim_num_insn", NULL);

  stat_reg_counter(sdb, "sim_num_refs",
           "total number of loads and stores executed",
           &sim_num_refs, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
           "total simulation time in seconds",
           &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
           "simulation speed (in insts/sec)",
           "sim_num_insn / sim_elapsed_time", NULL);
  ld_reg_stats(sdb);
  mem_reg_stats(mem, sdb);
}

/* initialize the simulator */
void
sim_init(void)
{
  sim_num_refs = 0;

  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* load program into simulated state */
void
sim_load_prog(char *fname,      /* program to load */
          int argc, char **argv,    /* program arguments */
          char **envp)      /* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)        /* output stream */
{
  /* nothing currently */
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)     /* output stream */
{
  /* nada */
}

/* un-initialize simulator-specific state */
void
sim_uninit(void)
{
  /* nada */
}


/*
 * configure the execution engine
 */

/*
 * precise architected register accessors
 */

/* next program counter */
#define SET_NPC(EXPR)       (regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC         (regs.regs_PC)

/* general purpose registers */
#define GPR(N)          (regs.regs_R[N])
#define SET_GPR(N,EXPR)     (regs.regs_R[N] = (EXPR))

#if defined(TARGET_PISA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_L(N)        (regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)   (regs.regs_F.l[(N)] = (EXPR))
#define FPR_F(N)        (regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)   (regs.regs_F.f[(N)] = (EXPR))
#define FPR_D(N)        (regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)   (regs.regs_F.d[(N) >> 1] = (EXPR))

/* miscellaneous register accessors */
#define SET_HI(EXPR)        (regs.regs_C.hi = (EXPR))
#define HI          (regs.regs_C.hi)
#define SET_LO(EXPR)        (regs.regs_C.lo = (EXPR))
#define LO          (regs.regs_C.lo)
#define FCC         (regs.regs_C.fcc)
#define SET_FCC(EXPR)       (regs.regs_C.fcc = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)                       \
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_BYTE(mem, addr))
#define READ_HALF(SRC, FAULT)                       \
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_HALF(mem, addr))
#define READ_WORD(SRC, FAULT)                       \
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_WORD(mem, addr))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)                      \
  ((FAULT) = md_fault_none, addr = (SRC), MEM_READ_QWORD(mem, addr))
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)                 \
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_BYTE(mem, addr, (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)                 \
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_HALF(mem, addr, (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)                 \
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_WORD(mem, addr, (SRC)))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)                    \
  ((FAULT) = md_fault_none, addr = (DST), MEM_WRITE_QWORD(mem, addr, (SRC)))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)   sys_syscall(&regs, mem_access, mem, INST, TRUE)

#define DNA         (0)

/* general register dependence decoders */
#define DGPR(N)         (N)
#define DGPR_D(N)       ((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)       (((N)+32)&~1)
#define DFPR_F(N)       (((N)+32)&~1)
#define DFPR_D(N)       (((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DHI         (0+32+32)
#define DLO         (1+32+32)
#define DFCC            (2+32+32)
#define DTMP            (3+32+32)


#define IFQ_SIZE 8

// dispatch: IFQ -> ROB
// issue: ROB -> FU
// retire: ROB -> nothing
#define ROB_SIZE 32

#define BRANCH_PENALTY 3

// instruction status
enum instruction_status {
    ALLOCATED,
    FETCHED,
    DISPATCHED,
    EXECUTED,
    RETIRED
};

// the instruction type
typedef struct Inst {
    unsigned     uid;       // instruction number
    unsigned     pc;	    // instruction address
    unsigned	 next_pc;   // next instruction address
    md_inst_t    inst;      // instruction bits from memory
    enum md_opcode op;	    // opcode
    int          status;    // where is the instruction in the pipeline?
    unsigned     donecycle; // cycle when destination operand(s) generated (not necessarily when retired)
    struct Inst *next;      // used for custom memory allocation/deallocation
    // destination registers
    int dst1;            
    int dst2;            
    // ROB entry for results dependency
    int src1;             
    int src2;             
    int src3;             
} inst_t;

// Create register alias table
int rat[MD_TOTAL_REGS] = {-1};

// queue of fetched instructions
inst_t *g_ifq[IFQ_SIZE];
static int g_ifq_tail, g_ifq_head, g_ifq_count;

// reorder buffer + unified reservation station
inst_t *g_rob[ROB_SIZE];
static int g_rob_tail, g_rob_head, g_rob_count;

// track register dependencies
inst_t   *g_scoreboard[MD_TOTAL_REGS];
md_addr_t g_fetch_pc = 0;
unsigned g_uid = 1;

unsigned int fetch_resume_cycle = 0;

// functional unit resource pool
struct res_desc fu_config[] = {
  {
    "integer-ALU",
    4,
    0,
    {
      { IntALU, 1, 1 }
    }
  },
  {
    "integer-MULT/DIV",
    1,
    0,
    {
      { IntMULT, 3, 1 },
      { IntDIV, 20, 19 }
    }
  },
  {
    "memory-port",
    1,
    0,
    {
      { RdPort, 1, 1 },
      { WrPort, 1, 1 }
    }
  },
  {
    "FP-adder",
    4,
    0,
    {
      { FloatADD, 2, 1 },
      { FloatCMP, 2, 1 },
      { FloatCVT, 2, 1 }
    }
  },
  {
    "FP-MULT/DIV",
    1,
    0,
    {
      { FloatMULT, 4, 1 },
      { FloatDIV, 12, 12 },
      { FloatSQRT, 24, 24 }
    }
  },
};

static struct res_pool *fu_pool = NULL;

int rand_load_lat()
{
    int lat = 1;
    double p = ((double) myrand())/((double) RAND_MAX);

    if (p < 0.05) {
        lat += 15; // L1 miss
        if (p < 0.005) {
            lat += 200; // L2 miss
        }
    }
    return lat;
}

static void cpen411_init()
{
    int i;
    fprintf(stderr, "sim: ** starting CPEN 411 out-of-order simulation **\n");

    // empty IFQ
    g_ifq_head = g_ifq_tail = g_ifq_count = 0;
    g_rob_head = g_rob_tail = g_rob_count = 0;

    // functional units
    fu_pool = res_create_pool("fu-pool", fu_config, N_ELT(fu_config));

    // SB
    for (i = 0; i < MD_TOTAL_REGS; ++i)
        g_scoreboard[i] = NULL;

    /* set up initial default next PC */
    g_fetch_pc = regs.regs_PC;
    regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);
}

static void flush_ifq()
{
    while (g_ifq_count != 0) {
        inst_t *pI = g_ifq[g_ifq_head];
        g_ifq_head = (g_ifq_head + 1) % IFQ_SIZE;
        g_ifq_count--;
        free(pI);
    }
}

static void fetch(void)
{
    if (fetch_resume_cycle > sim_cycle) // mispredicted branch
        return;

    if (g_ifq_count < IFQ_SIZE) {
        md_inst_t inst;
        inst_t *pI = NULL;

        // allocate an instruction record, fill in basic information
        pI            = malloc(sizeof(inst_t));
        pI->pc        = g_fetch_pc;
        pI->status    = ALLOCATED;
        pI->op        = 0;
        pI->donecycle = 0xFFFFFFFF; // i.e., largest unsigned integer
        pI->uid       = g_uid++;
        pI->src1    = DNA;
        pI->src2    = DNA;
        pI->src3    = DNA;
        pI->dst1   = DNA;
        pI->dst2   = DNA;

        MD_FETCH_INST(inst, mem, g_fetch_pc);
        pI->inst = inst;
        g_ifq[g_ifq_tail] = pI;
        g_ifq_tail = (g_ifq_tail + 1) % IFQ_SIZE;
        g_ifq_count++;

        g_fetch_pc += sizeof(md_inst_t);
    }
}

static void dispatch(void)
{
    if ((g_ifq_count > 0) && (g_rob_count < ROB_SIZE )) {
        inst_t *pI = g_ifq[g_ifq_head];
        md_inst_t inst = pI->inst;
        enum md_opcode op;
        MD_SET_OPCODE(op, inst);
        pI->op = op;

        g_rob[g_rob_tail] = pI;
        g_rob_tail = (g_rob_tail + 1) % ROB_SIZE;
        g_rob_count++;
        g_ifq_head = (g_ifq_head + 1) % IFQ_SIZE;
        g_ifq_count--;

        pI->status = DISPATCHED;

        // BEGIN FUNCTIONAL EXECUTION -->

        assert( pI->pc == regs.regs_PC );

        register md_addr_t addr;
        enum md_fault_type fault;
        int i1, i2, i3, o1, o2;

        /* maintain $r0 semantics */
        regs.regs_R[MD_REG_ZERO] = 0;
        sim_num_insn++;
        /* set default reference address, access mode, and fault */
        addr = 0; fault = md_fault_none;

        /* execute the instruction */
        switch (op)
        {
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)    \
        case OP:                                                    \
              i1 = I1; i2 = I2; i3 = I3; o1 = O1; o2 = O2;          \
              SYMCAT(OP,_IMPL);                                     \
              break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)                         \
            case OP:                                                \
              panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)                                    \
          { fault = (FAULT); break; }
#include "machine.def"
        default:
          panic("attempted to execute a bogus opcode");
        }

        if (fault != md_fault_none)
            fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

        if (verbose) {
            myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
            sim_num_insn, md_xor_regs(&regs), regs.regs_PC);
            md_print_insn(inst, regs.regs_PC, stderr);
            if (MD_OP_FLAGS(op) & F_MEM)
            myfprintf(stderr, "  mem: 0x%08p", addr);
            fprintf(stderr, "\n");
            /* fflush(stderr); */
        }
        if (MD_OP_FLAGS(op) & F_MEM) sim_num_refs++;

        /* go to the next instruction */
        regs.regs_PC = regs.regs_NPC;
        regs.regs_NPC += sizeof(md_inst_t);

        // <---  END FUNCTIONAL EXECUTION

        // record correct next instruction address (use for branches/jumps)
        pI->next_pc = regs.regs_PC;

        // TODO: model dynamic dependencies
        // TODO: we have conveniently decoded i1, i2, i3, o1, and o2 for you

        //Rename source registers using alias table
        if(i1 != DNA && rat[i1] != -1){
          pI->src1 = rat[i1];
        }

        if(i2 != DNA && rat[i2] != -1){
          pI->src2 = rat[i2];
        }

        if(i3 != DNA && rat[i3] != -1){
          pI->src3 = rat[i3];
        }

        if(o1 != DNA){
          rat[o1] = g_rob_tail;
          pI->dst1 = g_rob_tail;
        }
        if(o2 != DNA){
          rat[o2] = g_rob_tail;
          pI->dst2 = g_rob_tail;
        }

        // branches
        if (MD_OP_FLAGS(op) & F_CTRL)  {
            int taken = (regs.regs_PC != (pI->pc+sizeof(md_inst_t)));
            if (taken) {
                fetch_resume_cycle = 0xFFFFFFFF; // unresolved
                g_fetch_pc = regs.regs_PC;
                flush_ifq();
            }
        }
    }
}

static void issue_and_execute()
{
    int i;
    // compute execution latency if FU available (entire ROB searched to simplify)
    for (i = 0; i < g_rob_count; ++i) {
        inst_t *pI = g_rob[(g_rob_head + i) % ROB_SIZE];
        if (pI->status == DISPATCHED) { // not yet executed
            // int have_all_operands = 1; // XXX only works with "issue only on ROB empty" baseline
            // TODO: check dynamically whether we have all operands, adjust have_all_operands
            int have_all_operands = (pI->src1 == DNA) && (pI->src2 == DNA) && (pI->src3 == DNA);

            if (have_all_operands) {
                if (MD_OP_FUCLASS(pI->op) != NA) {
                    struct res_template *fu;
                    fu = res_get(fu_pool, MD_OP_FUCLASS(pI->op));
                    if (fu) { // we have a free FU
                        if (fu->master->busy) panic("FU already in use");
                        fu->master->busy = fu->issuelat;
                        pI->status = EXECUTED;
                        pI->donecycle = sim_cycle + fu->oplat;
                        if ((MD_OP_FLAGS(pI->op) & F_CTRL) && (fetch_resume_cycle == 0xFFFFFFFF))  {
                            fetch_resume_cycle = pI->donecycle + BRANCH_PENALTY;
                        }
                        if (MD_OP_FLAGS(pI->op) & (F_MEM|F_LOAD)) {
                            pI->donecycle += rand_load_lat();
                        }
                    }
                } else { // no functional unit required, e.g., jr
                    pI->status = EXECUTED;
                    pI->donecycle = sim_cycle;
                    if ((MD_OP_FLAGS(pI->op) & F_CTRL) && (fetch_resume_cycle == 0xFFFFFFFF))  {
                        fetch_resume_cycle = pI->donecycle + BRANCH_PENALTY;
                    }
                }
            }
        }
    }
}

static void advance_fus(void)
{
    int i;
    for (i=0; i<fu_pool->num_resources; i++) {
        if (fu_pool->resources[i].busy > 0)
            fu_pool->resources[i].busy--;
    }
}

void retire(void)
{
    if (g_rob_count > 0) {
        inst_t *pI = g_rob[g_rob_head];
        if( pI->donecycle > sim_cycle ) // ROB head not ready to retire
            return;

        g_rob_head = (g_rob_head + 1) % ROB_SIZE;
        g_rob_count--;

        // instruction has finished executing

        // TODO: resolve outstanding dependencies
        for(int i = 0;i < MD_TOTAL_REGS;i++)
        {
          if(rat[i] == pI->dst1){ 
            rat[i] = -1;
          }
          if(rat[i] == pI->dst2){ 
            rat[i] = -1;
          }
        }

        //Check ROB and clear dependency on destination register
        for (int i = 0; i < g_rob_count; ++i) {
          inst_t *dep = g_rob[(g_rob_head + i) % ROB_SIZE];

          if(dep->src1 == pI->dst2 || dep->src1 == pI->dst1){
            dep->src1 = DNA;
          }
          if(dep->src2 == pI->dst2 || dep->src2 == pI->dst1){
            dep->src2 = DNA;
          }
          if(dep->src3 == pI->dst2 || dep->src3 == pI->dst1){
            dep->src3 = DNA;
          }
        }

        pI->status = RETIRED;
        free(pI);
    }

}

/* start simulation, program loaded, processor precise state initialized */
void sim_main(void)
{
    cpen411_init();

    do {
        advance_fus();
        advance_fus();
        advance_fus();
        advance_fus();
        advance_fus();
        advance_fus();
        advance_fus();
        advance_fus();
        retire();
        retire();
        retire();
        retire();
        retire();
        retire();
        retire();
        retire();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        issue_and_execute();
        dispatch();
        dispatch();
        dispatch();
        dispatch();
        dispatch();
        dispatch();
        dispatch();
        dispatch();
        fetch();
        fetch();
        fetch();
        fetch();
        fetch();
        fetch();
        fetch();
        fetch();

        sim_cycle++;
    } while (!max_insts || sim_num_insn < max_insts);
}
