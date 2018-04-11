/* <iodef.h>
 *
 *	I/O function code definitions
 */
#ifndef _IODEF_H
#define _IODEF_H

/* *** start physical I/O function codes *** */
#define IO$_NOP		0	/*no operation */
#define IO$_UNLOAD	1	/*unload drive */
#define IO$_LOADMCODE	1	/*load microcode */
#define IO$_START_BUS	1	/*start LAVC bus */
#define IO$_SEEK	2	/*seek cylinder */
#define IO$_SPACEFILE	2	/*space files */
#define IO$_STARTMPROC	2	/*start microprocessor */
#define IO$_STOP_BUS	2	/*stop LAVC bus */
#define IO$_RECAL	3	/*recalibrate drive */
#define IO$_DUPLEX	3	/*enter duplex mode */
#define IO$_STOP	3	/*stop */
#define IO$_DEF_COMP	3	/*define network component */
#define IO$_DRVCLR	4	/*drive clear */
#define IO$_INITIALIZE	4	/*initialize */
#define IO$_MIMIC	4	/*enter mimic mode */
#define IO$_DEF_COMP_LIST 4	/*define network component list */
#define IO$_RELEASE	5	/*release port */
#define IO$_SETCLOCKP	5	/*set clock (physical) */
#define IO$_START_ANALYSIS 5	/*start LAVC failure analysis */
#define IO$_OFFSET	6	/*offset read heads */
#define IO$_ERASETAPE	6	/*erase tape */
#define IO$_STARTDATAP	6	/*start data transfer (physical) */
#define IO$_STOP_ANALYSIS 6	/*stop LAVC failure analysis */
#define IO$_RETCENTER	7	/*return to centerline */
#define IO$_QSTOP	7	/*queue stop request */
#define IO$_START_MONITOR 7	/*start LAVC channel monitor. */
#define IO$_PACKACK	8	/*pack acknowledge */
#define IO$_STOP_MONITOR 8	/*stop LAVC channel monitor. */
#define IO$_SEARCH	9	/*search for sector */
#define IO$_SPACERECORD 9	/*space records */
#define IO$_READRCT	9	/*read replacement & caching table */
#define IO$_WRITECHECK	10	/*write check data */
#define IO$_WRITEPBLK	11	/*write physical block */
#define IO$_READPBLK	12	/*read physical block */
#define IO$_WRITEHEAD	13	/*write header and data */
#define IO$_RDSTATS	13	/*read statistics */
#define IO$_CRESHAD	13	/*create a shadow set */
#define IO$_READHEAD	14	/*read header and data */
#define IO$_ADDSHAD	14	/*add member to shadow set */
#define IO$_WRITETRACKD 15	/*write track data */
#define IO$_COPYSHAD	15	/*perform shadow set copy operations */
#define IO$_READTRACKD	16	/*read track data */
#define IO$_REMSHAD	16	/*remove member from shadow set */
#define IO$_AVAILABLE	17	/*available (disk and tape class) */
#define IO$_SETPRFPATH	18	/*set preferred path */
#define IO$_DISPLAY	19	/*display volume label */
#define IO$_DSE		21	/*data security erase (and rewind) */
#define IO$_REREADN	22	/*reread next */
#define IO$_DISK_COPY_DATA 22	/*disk_copy_data */
#define IO$_REREADP	23	/*reread previous */
#define IO$_AS_SETCHAR	23	/*Asian set characteristics */
#define IO$_WRITERET	24	/*write retry */
#define IO$_WRITECHECKH 24	/*write check header and data */
#define IO$_AS_SENSECHAR 24	/*Asian sense characteristics */
#define IO$_READPRESET	25	/*readin preset */
#define IO$_STARTSPNDL	25	/*start spindle */
#define IO$_SETCHAR	26	/*set characteristics */
#define IO$_SENSECHAR	27	/*sense tape characteristics */
#define IO$_WRITEMARK	28	/*write tape mark */
#define IO$_COPYMEM	28	/*copy memory */
#define IO$_PSXSETCHAR	28	/*POSIX set characteristics */
#define IO$_WRTTMKR	29	/*write tape mark retry */
#define IO$_DIAGNOSE	29	/*diagnose */
#define IO$_SHADMV	29	/*preform mount ver. on shadow set */
#define IO$_PSXSENSECHAR 29	/*POSIX sense characteristics */
#define IO$_FORMAT	30	/*format */
#define IO$_CLEAN	30	/*clean tape */
#define IO$_UPSHAD	30	/*up date shad lock ID */
#define IO$_PHYSICAL	31	/*highest physical I/O function code */
/* *** start logical I/O function codes *** */
#define IO$_WRITELBLK	32	/*write logical block */
#define IO$_READLBLK	33	/*read logical block */
#define IO$_REWINDOFF	34	/*rewind and set offline */
#define IO$_READRCTL	34	/*read RCT sector zero */
#define IO$_SETMODE	35	/*set mode */
#define IO$_REWIND	36	/*rewind tape */
#define IO$_SKIPFILE	37	/*skip files */
#define IO$_PSXSETMODE	37	/*POSIX set mode */
#define IO$_SKIPRECORD	38	/*skip records */
#define IO$_PSXSENSEMODE 38	/*POSIX sense mode */
#define IO$_SENSEMODE	39	/*sense tape mode */
#define IO$_WRITEOF	40	/*write end of file */
#define IO$_TTY_PORT_BUFIO 40	/*buffered I/O terminal port FDT routine */
#define IO$_TTY_PORT	41	/*terminal port FDT routine */
#define IO$_FREECAP	41	/*return free capacity */
#define IO$_FLUSH	42	/*flush controller cache */
#define IO$_AS_SETMODE	42	/*Asian set mode */
#define IO$_READLCHUNK	43	/*read large logical block */
#define IO$_AS_SENSEMODE 43	/*Asian sense mode */
#define IO$_WRITELCHUNK 44	/*write large logical block */
#define IO$_LOGICAL	47	/*highest logical I/O function code */
/* *** start virtual I/O function codes */
#define IO$_WRITEVBLK	48	/*write virtual block */
#define IO$_READVBLK	49	/*read virtual block */
#define IO$_ACCESS	50	/*access file */
#define IO$_PSXWRITEVBLK 50	/*POSIX write virtual */
#define IO$_CREATE	51	/*create file */
#define IO$_DEACCESS	52	/*deaccess file */
#define IO$_PSXREADVBLK 52	/*POSIX read virtual */
#define IO$_DELETE	53	/*delete file */
#define IO$_MODIFY	54	/*modify file */
#define IO$_NETCONTROL	54	/*X25 network control function */
#define IO$_READPROMPT	55	/*read terminal with prompt */
#define IO$_SETCLOCK	55	/*set clock */
#define IO$_ACPCONTROL	56	/*miscellaneous ACP control */
#define IO$_STARTDATA	56	/*start data */
#define IO$_MOUNT	57	/*mount volume */
#define IO$_TTYREADALL	58	/*terminal read passall */
#define IO$_TTYREADPALL 59	/*term read w/prompt passall */
#define IO$_CONINTREAD	60	/*connect to interrupt readonly */
#define IO$_CONINTWRITE 61	/*connect to interrupt with write */
#define IO$_VIRTUAL	63	/*highest virtual I/O function */
/* Function modifier bit definitions */
#define IO$M_FCODE	0x003F
#define IO$M_FMODIFIERS 0xFFC0
#define IO$M_INHERLOG	0x0800
#define IO$M_ERASE	0x0400
#define IO$V_ENCRYPT 13			/* Encryption */
#define IO$M_ENCRYPT	0x2000
#define IO$M_EXFUNC	0x2000
#define IO$M_DATACHECK	0x4000
#define IO$M_INHRETRY	0x8000
#define IO$M_SYNCSTS	0x00010000
#define IO$M_NOVCACHE	0x00020000
#define IO$M_INHSEEK	0x1000
#define IO$M_REVERSE	0x0040
#define IO$M_NOWAIT	0x0080
#define IO$M_INHEXTGAP	0x1000
#define IO$M_RETENSION	0x2000
#define IO$M_MSCPMODIFS 0x0100
#define IO$M_SHADOW	0x0040
#define IO$M_LOCATE	0x0080
#define IO$M_MSCP_FORMAT 0x0040
#define IO$M_ALLHOSTS	0x0040
#define IO$M_DISSOLVE	0x0080
#define IO$M_NOCLEANUP	0x0100
#define IO$M_SPINDOWN	0x0040
#define IO$M_FORCEPATH	0x0040
#define IO$M_STEPOVER	0x0040
#define IO$M_COPYOP	0x0100
#define IO$M_EXISTS	0x0040
#define IO$M_CBS	0x0080
#define IO$M_BOOTING	0x0100
#define IO$M_VUEX_FC	0x0200
#define IO$M_COMMOD	0x0040
#define IO$M_MOVETRACKD 0x0080
#define IO$M_DIAGNOSTIC 0x0100
#define IO$M_SKPSECINH	0x0200
#define IO$M_DELDATA	0x0040
#define IO$M_NOMRSP	0x0040
#define IO$M_SWAP	0x0100
#define IO$M_OPPOSITE	0x0200
#define IO$M_CLSEREXCP	0x0200
#define IO$M_CHUNKDIAG	0x0040
#define IO$M_TBC	0x0400
#define IO$M_ENAREP	0x0040
#define IO$M_ACCESS	0x0040
#define IO$M_CREATE	0x0080
#define IO$M_DELETE	0x0100
#define IO$M_MOUNT	0x0200
#define IO$M_DMOUNT	0x0400
#define IO$M_REMOUNT	0x0800
#define IO$M_MOVEFILE	0x1000
#define IO$M_RWSHELVED	0x4000
#define IO$M_BINARY	0x0040
#define IO$M_PACKED	0x0080
#define IO$M_NOW	0x0040
#define IO$M_STREAM	0x0080
#define IO$M_READERCHECK 0x0100
#define IO$M_WRITERCHECK 0x0200
#define IO$M_NORSWAIT	0x0400
#define IO$M_MB_ROOM_NOTIFY 0x0040
#define IO$M_READATTN	0x0080
#define IO$M_WRTATTN	0x0100
#define IO$M_SETPROT	0x0200
#define IO$M_READERWAIT 0x0400
#define IO$M_WRITERWAIT 0x0800
#define IO$M_NOECHO	0x0040
#define IO$M_TIMED	0x0080
#define IO$M_CVTLOW	0x0100
#define IO$M_NOFILTR	0x0200
#define IO$M_DSABLMBX	0x0400
#define IO$M_PURGE	0x0800
#define IO$M_TRMNOECHO	0x1000
#define IO$M_REFRESH	0x2000
#define IO$M_ESCAPE	0x4000
#define IO$M_EXTEND	0x8000
#define IO$M_CANCTRLO	0x0040
#define IO$M_ENABLMBX	0x0080
#define IO$M_NOFORMAT	0x0100
#define IO$M_BREAKTHRU	0x0200
#define IO$M_NEWLINE	0x0400
#define IO$M_TYPEAHDCNT 0x0040
#define IO$M_MAINT	0x0040
#define IO$M_CTRLYAST	0x0080
#define IO$M_CTRLCAST	0x0100
#define IO$M_HANGUP	0x0200
#define IO$M_OUTBAND	0x0400
#define IO$M_TT_CONNECT 0x0800
#define IO$M_TT_DISCON	0x1000
#define IO$M_TT_PROCESS 0x2000
#define IO$M_BRDCST	0x4000
#define IO$M_LOOP	0x0080
#define IO$M_UNLOOP	0x0100
#define IO$M_LINE_OFF	0x0200
#define IO$M_SET_MODEM	0x0400
#define IO$M_LINE_ON	0x0800
#define IO$M_LOOP_EXT	0x1000
#define IO$M_AUTXOF_ENA 0x2000
#define IO$M_AUTXOF_DIS 0x4000
#define IO$M_INCLUDE	0x0800
#define IO$M_TT_ABORT	0x1000
#define IO$M_POSIXINIT	0x0040
#define IO$M_POSIXFLOW	0x0080
#define IO$M_SET_POSIX	0x0080
#define IO$M_CLEAR_POSIX 0x0100
#define IO$M_SET_TERMIOS 0x0200
#define IO$M_SET_PTC	0x0400
#define IO$M_CLEAR_PTC	0x0800
#define IO$M_FLUSH_TAB	0x1000
#define IO$M_FLUSH_OUTPUT 0x2000
#define IO$M_UPDATE_PTC 0x4000
#define IO$M_OUT_XOFF	0x0100
#define IO$M_OUT_XON	0x0200
#define IO$M_IN_XOFF	0x0400
#define IO$M_IN_XON	0x0800
#define IO$M_O_NONBLOCK 0x0040
#define IO$M_INTERRUPT	0x0040
#define IO$M_MULTIPLE	0x0080
#define IO$M_LOCKBUF	0x0100
#define IO$M_NOBLOCK	0x0200
#define IO$M_ABORT	0x0100
#define IO$M_SYNCH	0x0200
#define IO$M_RESPONSE	0x0040
#define IO$M_STARTUP	0x0040
#define IO$M_SHUTDOWN	0x0080
#define IO$M_ATTNAST	0x0100
#define IO$M_CTRL	0x0200
#define IO$M_RD_MEM	0x0040
#define IO$M_RD_MODEM	0x0080
#define IO$M_RD_COUNT	0x0100
#define IO$M_CLR_COUNT	0x0400
#define IO$K_SRRUNOUT	0		/* send or rcv until cnt runout */
#define IO$K_PTPBSC	8192		/* point to point BSC control */
#define IO$K_LOOPTEST	57344		/* loop test */
/* X25 driver WRITEBLK function modifier bits. */
#define IO$M_MORE	0x0040
#define IO$M_QUALIFIED	0x0080
#define IO$M_REDIRECT	0x0040
#define IO$M_ACCEPT	0x0080
#define IO$M_SETEVF	0x0040
#define IO$M_WORD	0x0040
#define IO$M_SETFNCT	0x0200
#define IO$M_DATAPATH	0x0400
#define IO$M_CYCLE	0x1000
#define IO$M_RESET	0x2000
#define IO$M_SETCUADR	0x0100
#define IO$M_SETBSIZE	0x0200
#define IO$M_SETPOOLSZ	0x0400
#define IO$M_SETENQCNT	0x0800
#define IO$M_CLEAR	0x1000
#define IO$M_LPBEXT	0x2000
#define IO$M_LPBINT	0x4000
#define IO$M_READCSR	0x8000
#define IO$M_NOCTSWAIT	0x0040
#define IO$M_SLAVLOOP	0x0080
#define IO$M_NODSRWAIT	0x0100
#define IO$M_MAINTLOOP	0x0200
#define IO$M_LASTBLOCK	0x0400
#define IO$M_INTCLOCK	0x1000
#define IO$M_LT_CONNECT   0x0040
#define IO$M_LT_DISCON	  0x0080
#define IO$M_LT_READPORT  0x0100
#define IO$M_LT_MAP_PORT  0x0200
#define IO$M_LT_RATING	  0x0400
#define IO$M_LT_SOL_INFO  0x0800
#define IO$M_LT_RCV_INFO  0x1000
#define IO$M_LT_SETMODE   0x2000
#define IO$M_LT_SENSEMODE 0x4000
#define IO$M_LT_QUE_CHG_NOTIF 0x8000
#define IO$V_LT_MAP_FILLER 0
#define IO$V_LT_MAP_NODNAM 1
#define IO$V_LT_MAP_PORNAM 2
#define IO$V_LT_MAP_SRVNAM 3
#define IO$V_LT_MAP_LNKNAM 4
#define IO$V_LT_MAP_NETADR 5
#define IO$M_LT_MAP_FILLER (1<<IO$V_LT_MAP_FILLER) /* 0x01 */
#define IO$M_LT_MAP_NODNAM (1<<IO$V_LT_MAP_NODNAM) /* 0x02 */
#define IO$M_LT_MAP_PORNAM (1<<IO$V_LT_MAP_PORNAM) /* 0x04 */
#define IO$M_LT_MAP_SRVNAM (1<<IO$V_LT_MAP_SRVNAM) /* 0x08 */
#define IO$M_LT_MAP_LNKNAM (1<<IO$V_LT_MAP_LNKNAM) /* 0x10 */
#define IO$M_LT_MAP_NETADR (1<<IO$V_LT_MAP_NETADR) /* 0x20 */

/* these WS pseudo-device SETMODE modifiers are not in Starlet */
#define IO$V_WS_DISPLAY 6
#define IO$V_WS_PERM	7
#define IO$M_WS_DISPLAY (1<<IO$V_WS_DISPLAY)	/* 0x0040 */
#define IO$M_WS_PERM	(1<<IO$V_WS_PERM)	/* 0x0080 */

union iodef {
/* Basic I/O function code/modifiers structure */
    struct {
	unsigned io$v_fcode : 6;	/* function Code Field */
	unsigned io$v_fmodifiers : 10;	/* function Modifiers Field */
    } io$r_fcode_structure;
/* Function modifiers common to all drivers which do error logging */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_filler	 : 5;
	unsigned io$v_inherlog	 : 1;	/* inhibit error logging */
	unsigned		 : 4;
    } io$r_errlog_modifiers;
/* General disk and tape function code modifiers */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_rsvd_for_devdepend_1 : 4;
	unsigned io$v_erase	 : 1;	/* erase data */
	unsigned io$v_filler	 : 1;	/* IO$M_INHERLOG */
	unsigned io$v_rsvd_for_devdepend_2 : 1;
	unsigned io$v_exfunc	 : 1;	/* func<31:16> are valid */
	unsigned io$v_datacheck  : 1;	/* write check data after transfer */
	unsigned io$v_inhretry	 : 1;	/* inhibit error retry */
/* the following are only valid if EXFUNC is set */
	unsigned io$v_syncsts	 : 1;	/* allow synchronous completion */
	unsigned io$v_novcache	 : 1;	/* do not use VBN cache */
	unsigned		 : 6;
    } io$r_disk_tape_modifiers;
/* General disk function code modifiers */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_rsvd_for_devdepend : 6;
	unsigned io$v_inhseek	 : 1;	/* inhibit implied seek on physical functions */
	unsigned		 : 3;
    } io$r_disk_modifiers;
/* General tape function code modifiers */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reverse	 : 1;	/* reverse operation */
	unsigned io$v_nowait	 : 1;	/* no wait for rewind to complete */
	unsigned io$v_rsvd_for_devdepend : 4;
	unsigned io$v_inhextgap  : 1;	/* inhibit extended inter-record gap */
	unsigned io$v_retension  : 1;	/* traverse the entire tape to uniformly set the tape's tension; */
					/* applies only to IO$_REWIND/REWINDOFF functions. */
	unsigned		 : 2;
    } io$r_tape_modifiers;
/* DU  & TU, disk & tape class driver transfer function modifier bits */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_dutux_reserved : 2;
	unsigned io$v_mscpmodifs : 1;	/* use MSCP modifiers in P5 */
	unsigned		 : 7;
    } io$r_dutu_xfer_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_shadow	 : 1;	/* shadowing SENSECHAR */
	unsigned io$v_locate	 : 1;	/* shadowing Locate function */
    } io$r_du_sense_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_mscp_format : 1;	/* PACKACK media for formatting */
	unsigned		 : 1;
    } io$r_du_packack_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_allhosts	 : 1;	/* do available as for all class drivers */
	unsigned io$v_dissolve	 : 1;	/* dissolve shadow set */
	unsigned io$v_nocleanup  : 1;	/* normal shadow set cleanup not necessary */
	unsigned		 : 7;
    } io$r_du_avail_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_spindown	 : 1;	/* spin down removed member */
	unsigned		 : 1;
    } io$r_du_remshad_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_forcepath  : 1;	/* make path change happen */
	unsigned		 : 1;
    } io$r_du_prfpath_modifiers;
/* HBS driver transfer function modifier bits */
/* Make sure that IO$V_SHADOW .ne. IO$V_LOCATE .ne. IO$V_COPYOP */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_stepover	 : 1;	/* stepover SCBLBN. */
	unsigned io$v_cs_fill	 : 1;
	unsigned io$v_copyop	 : 1;	/* Shad Server Cpy Operation */
	unsigned		 : 7;
    } io$r_hbs_copyshad_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_exists	 : 1;	/* VU already exists. */
	unsigned io$v_cbs	 : 1;	/* controller based shadowing. */
	unsigned io$v_booting	 : 1;	/* creshad for system disk. */
	unsigned		 : 7;
    } io$r_hbs_creshad_modifiers;
    struct {
	unsigned io$v_fcode	 : 9;
	unsigned io$v_vuex_fc	 : 1;	/* exclude Full copy members */
	unsigned		 : 6;
    } io$r_hbs_xfer_modifiers;
/* DR driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_commod	 : 1;	/* diagnostic command */
	unsigned io$v_movetrackd : 1;	/* move track descriptor */
	unsigned io$v_diagnostic : 1;	/* diagnostic function */
	unsigned io$v_skpsecinh  : 1;	/* skip sector inhibit */
	unsigned		 : 6;
    } io$r_dr_modifiers;
/* DY driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_deldata	 : 1;	/* write deleted data mark */
	unsigned		 : 1;
    } io$r_dy_modifiers;
/* DD driver (TU58) function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_nomrsp	 : 1;	/* do not use MRSP for this operation */
	unsigned		 : 1;
    } io$r_dd_modifiers;
/* Magnetic tape I/O function modifier bits for the TS11. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_rsvd_for_devdepend : 2;
	unsigned io$v_swap	 : 1;	/* swap byte (TS11) */
	unsigned io$v_opposite	 : 1;	/* opposite bit for rereads (TS11) */
	unsigned		 : 6;
    } io$r_ts11_modifiers;
/* TU driver function modifier bits */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_rsvd_for_devdepend : 3;
	unsigned io$v_clserexcp  : 1;	/* clear serious exception condition */
	unsigned		 : 6;
    } io$r_tu_modifiers;
/* Tape class driver segmented and large transfer modifier bits */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_chunkdiag  : 1;	/* allow diagnostic buffers */
	unsigned io$v_rsvd_for_devdepend : 3;
	unsigned io$v_tbc	 : 1;	/* to be continued modifier */
	unsigned		 : 5;
    } io$r_tu_lg_transfer_modifiers;
/* TUDRIVER SETMODE function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_enarep	 : 1;	/* enable media quality reporting. */
	unsigned		 : 1;
    } io$r_tu_setmode_modifiers;
/* ACP function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_access	 : 1;	/* access file */
	unsigned io$v_create	 : 1;	/* create file */
	unsigned io$v_delete	 : 1;	/* delete file */
	unsigned io$v_mount	 : 1;	/* mount volume */
	unsigned io$v_dmount	 : 1;	/* dismount volume */
	unsigned io$v_remount	 : 1;	/* remount volume */
	unsigned io$v_movefile	 : 1;	/* move a file */
	unsigned		 : 1;
	unsigned io$v_rwshelved  : 1;	/* access shelved files */
	unsigned		 : 1;
    } io$r_acp_modifiers;
/* CR (card reader) driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_binary	 : 1;	/* binary read */
	unsigned io$v_packed	 : 1;	/* packed read */
    } io$r_cr_modifiers;
/* MB (mailbox) driver function modifier bits. */
/* The following modifers are for the READ, WRITE and SENSEMODE functions. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_now	 : 1;	/* do not wait for operation complete */
	unsigned io$v_stream	 : 1;	/* perform stream mode operation (vs. record mode) */
	unsigned io$v_readercheck : 1;	/* perform operation if no read channel is assigned to the mailbox. */
	unsigned io$v_writercheck : 1;	/* perform operation if no write channel is assigned to the mailbox. */
	unsigned io$v_norswait	 : 1;	/* do not allow resource waits */
	unsigned		 : 5;
    } io$r_mb_modifiers;
/* More MB (mailbox) driver function modifier bits. */
/* Mailbox QIO SETMODE function modifiers. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_mb_room_notify : 1; /* notify when some (any) space in the mailbox is available. */
	unsigned io$v_readattn	 : 1;	/* request AST on waiting reader */
	unsigned io$v_wrtattn	 : 1;	/* request AST on waiting writer */
	unsigned io$v_setprot	 : 1;	/* set volume protection */
	unsigned io$v_readerwait : 1;	/* wait for a read channel to be assigned to the mailbox. */
	unsigned io$v_writerwait : 1;	/* wait for a write channel to be assigned to the mailbox. */
	unsigned		 : 4;
    } io$r_mb_setmode_modifiers;
/* TT driver READ function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_noecho	 : 1;	/* noecho */
	unsigned io$v_timed	 : 1;	/* timed */
	unsigned io$v_cvtlow	 : 1;	/* convert lower case */
	unsigned io$v_nofiltr	 : 1;	/* no filter */
	unsigned io$v_dsablmbx	 : 1;	/* disable mailbox */
	unsigned io$v_purge	 : 1;	/* purge typeahead */
	unsigned io$v_trmnoecho  : 1;	/* terminators are not echoed */
	unsigned io$v_refresh	 : 1;	/* control-R interrupted read */
	unsigned io$v_escape	 : 1;	/* terminate read on escape sequence */
	unsigned io$v_extend	 : 1;	/* used by alternate class drivers */
    } IO$R_TT_READ_MODIFIERS;
/* TT driver WRITE function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_canctrlo	 : 1;	/* cancel control O */
	unsigned io$v_enablmbx	 : 1;	/* enable mailbox */
	unsigned io$v_noformat	 : 1;	/* do not format output */
	unsigned io$v_breakthru  : 1;	/* broadcast I/O */
	unsigned io$v_newline	 : 1;	/* output a newline */
	unsigned		 : 5;
    } io$r_tt_write_modifiers;
/* TT driver SENSEMODE function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_typeahdcnt : 1;	/* sense typeahead information */
	unsigned io$v_reserve_rd_modem : 1;
    } io$r_tt_sensemode_modifiers;
/* TT driver SETMODE subfunction modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_maint	 : 1;	/* enable maint sub modifiers */
	unsigned io$v_ctrlyast	 : 1;	/* set control Y AST */
	unsigned io$v_ctrlcast	 : 1;	/* set Control C */
	unsigned io$v_hangup	 : 1;	/* set mode and hang up line */
	unsigned io$v_outband	 : 1;	/* set out of band AST */
	unsigned io$v_tt_connect : 1;	/* connect to detached terminal */
	unsigned io$v_tt_discon  : 1;	/* disconnect detached terminal */
	unsigned io$v_tt_process : 1;	/* define controlling process */
	unsigned io$v_brdcst	 : 1;	/* define broadcast mask */
	unsigned		 : 1;
    } io$r_tt_setmode_modifiers;
/* TT driver MAINTIANCE subfunction submodifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reserve_maint_escape : 1;
	unsigned io$v_loop	 : 1;	/* diagnostic loopback */
	unsigned io$v_unloop	 : 1;	/* diagnostic reset loopback */
	unsigned io$v_line_off	 : 1;	/* disable line */
	unsigned io$v_set_modem  : 1;	/* diagnostic modem control */
	unsigned io$v_line_on	 : 1;	/* enable line */
	unsigned io$v_loop_ext	 : 1;	/* diagnostic external loopback */
	unsigned io$v_autxof_ena : 1;	/* enable auto XOFF */
	unsigned io$v_autxof_dis : 1;	/* disable auto XOFF */
	unsigned io$v_reserve_int_disable : 1;
    } io$r_tt_maint_modifiers;
/* TT driver out-of-band modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reserve_maint_escape : 5;
	unsigned io$v_include	 : 1;	/* include character in stream */
	unsigned io$v_tt_abort	 : 1;	/* abort current I/O */
	unsigned		 : 3;
    } io$r_tt_outofband_modifiers;
/* Posix terminal driver SETMODE subfunction modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_posixinit  : 1;	/* enable initialization subfunctions */
	unsigned io$v_posixflow  : 1;	/* enable flow control subfunctions */
    } io$r_tt_psxsetmode_modifiers;
/* Posix terminal driver INIT subfunction submodifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reserve_init_escape : 1;
	unsigned io$v_set_posix  : 1;	/* set TT3$M_POSIX and set up PTC */
	unsigned io$v_clear_posix : 1;	/* clear TT3$M_POSIX (opt. delete PTC) */
	unsigned io$v_set_termios : 1;	/* set up termios */
	unsigned io$v_set_ptc	 : 1;	/* set up all of PTC */
	unsigned io$v_clear_ptc  : 1;	/* clear PTC */
	unsigned io$v_flush_tab  : 1;	/* flush typeahead buffer */
	unsigned io$v_flush_output : 1; /* flush outstanding writes */
	unsigned io$v_update_ptc : 1;	/* update PTC */
	unsigned		 : 1;
    } io$r_tt_posixinit_modifiers;
/* Posix terminal driver FLOW subfunction submodifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reserve_flow_escape : 2;
	unsigned io$v_out_xoff	 : 1;	/* simulate output XOFF */
	unsigned io$v_out_xon	 : 1;	/* simulate output XON */
	unsigned io$v_in_xoff	 : 1;	/* simulate input XOFF */
	unsigned io$v_in_xon	 : 1;	/* simulate input XON */
	unsigned		 : 4;
    } io$r_tt_posixflow_modifiers;
/* Posix terminal driver READ function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_o_nonblock : 1;	/* nonblock */
	unsigned		 : 1;
    } io$r_tt_psxread_modifiers;
/* Network WRITE and READ VIRTUAL function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_interrupt  : 1;	/* interrupt message */
	unsigned io$v_multiple	 : 1;	/* multiple write/read request */
	unsigned io$v_lockbuf	 : 1;	/* direct I/O request */
	unsigned io$v_noblock	 : 1;	/* segment message w/o blocking */
	unsigned		 : 6;
    } io$r_net_write_read_modifiers;
/* Network ACCESS/DEACCESS function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_reserved	 : 2;
	unsigned io$v_abort	 : 1;	/* disconnect abort/connect reject */
	unsigned io$v_synch	 : 1;	/* synchronous disconnect */
	unsigned		 : 6;
    } io$r_net_accdea_modifiers;
/* DATALINK driver WRITE subfunction modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_response	 : 1;	/* transmit this packet as a response */
	unsigned		 : 1;
    } io$r_dlink_write_modifiers;
/* DATALINK driver SETMODE subfunction modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_startup	 : 1;	/* start protocol */
	unsigned io$v_shutdown	 : 1;	/* stop protocol */
	unsigned io$v_attnast	 : 1;	/* attention AST */
	unsigned io$v_ctrl	 : 1;	/* controller (ie. not station) function */
	unsigned io$v_reserve_set_modem : 1;
	unsigned		 : 5;
    } io$r_dlink_setmode_modifiers;
/* DATALINK driver SENSEMODE subfunction modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_rd_mem	 : 1;	/* read device memory */
	unsigned io$v_rd_modem	 : 1;	/* read modem status */
	unsigned io$v_rd_count	 : 1;	/* read counters */
	unsigned io$v_reserve_ctrl : 1;
	unsigned io$v_clr_count  : 1;	/* clear counters */
	unsigned		 : 5;
    } io$r_dlink_sensemode_modifiers;
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_more	 : 1;	/* more data follows (M-bit) */
	unsigned io$v_qualified  : 1;	/* use qualified sub-channel (Q-bit) */
    } io$r_x25_write_modifiers;
/* X25 driver ACCESS function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_redirect	 : 1;	/* redirect virtual call */
	unsigned io$v_accept	 : 1;	/* accept virtual call */
    } io$r_x25_access_modifiers;
/* LPA-11 driver STARTDATA modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_setevf	 : 1;	/* set event flag */
	unsigned		 : 1;
    } io$r_lpa_start_modifiers;
/* XA (DR11-W) driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_word	 : 1;	/* word (interrupt)/block (DMA) mode specifier */
	unsigned io$v_filler	 : 1;	/* IO$M_TIMED */
	unsigned io$v_filler2	 : 1;	/* IO$M_ATTNAST, IO$M_DIAGNOSTIC */
	unsigned io$v_setfnct	 : 1;	/* set "FNCT" bits in device CSR */
	unsigned io$v_datapath	 : 1;	/* change UBA datapath (direct/buffered) */
	unsigned io$v_filler3	 : 1;	/* IO$M_INHERLOG (former def'n for RESET) */
	unsigned io$v_cycle	 : 1;	/* set "cycle" bit in device CSR */
	unsigned io$v_reset	 : 1;	/* device reset specifier */
	unsigned		 : 2;
    } io$r_xa_modifiers;
/* 3271 driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_filler	 : 2;
	unsigned io$v_setcuadr	 : 1;	/* set a new CU address */
	unsigned io$v_setbsize	 : 1;	/* set a new maximum buffer size */
	unsigned io$v_setpoolsz  : 1;	/* set a new pool size */
	unsigned io$v_setenqcnt  : 1;	/* set a new ENQ threshold */
	unsigned io$v_clear	 : 1;	/* zero status counters */
	unsigned io$v_lpbext	 : 1;	/* loopback is external loopback */
	unsigned io$v_lpbint	 : 1;	/* loopback is internal loopback (DUP11) */
	unsigned io$v_readcsr	 : 1;	/* read CSRs on DUP11 */
    } io$r_ibm3271_modifiers;
/* XW driver function modifier bits. */
    struct {
	unsigned io$v_fcode	 : 6;
	unsigned io$v_noctswait  : 1;	/* diagnostic function */
	unsigned io$v_slavloop	 : 1;	/* diagnostic function */
	unsigned io$v_nodsrwait  : 1;	/* do not wait for DSR (diag) */
	unsigned io$v_maintloop  : 1;	/* internal maint loop */
	unsigned io$v_lastblock  : 1;	/* last block of message */
	unsigned io$v_filler	 : 1;
	unsigned io$v_intclock	 : 1;	/* internal clock */
	unsigned		 : 3;
    } io$r_xw_modifiers;
/* LAT driver function modifier bits. */
    struct {
	unsigned io$v_fcode	   : 6;
	unsigned io$v_lt_connect   : 1; /* solicit connection to remote LAT device */
	unsigned io$v_lt_discon    : 1; /* disconnect LAT session */
	unsigned io$v_lt_readport  : 1; /* read LAT device info */
	unsigned io$v_lt_map_port  : 1; /* map application port */
	unsigned io$v_lt_rating    : 1; /* set service rating */
	unsigned io$v_lt_sol_info  : 1; /* request solicit info */
	unsigned io$v_lt_rcv_info  : 1; /* setup to recive reply */
	unsigned io$v_lt_setmode   : 1; /* SETMODE on LTA device */
	unsigned io$v_lt_sensemode : 1; /* SENSEMODE on LTA device */
	unsigned io$v_lt_que_chg_notif : 1; /* queue change AST */
    } io$r_lt_modifiers;
/* LAT item list definitions for MAP_PORT */
    struct {
	unsigned io$v_lt_map_filler : 1; /* item 0 not used */
	unsigned io$v_lt_map_nodnam : 1; /* server node name */
	unsigned io$v_lt_map_pornam : 1; /* server port name */
	unsigned io$v_lt_map_srvnam : 1; /* server service name */
	unsigned io$v_lt_map_lnknam : 1; /* link name */
	unsigned io$v_lt_map_netadr : 1; /* server Ethernet Add. */
	unsigned		    : 2;
    } io$r_lt_mapitem;
};

#endif	/*_IODEF_H*/