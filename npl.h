/* O-Prolog
written by kenichi sasagawa 2016/9~
*/
#include <setjmp.h>
#if _WIN32
#include <windows.h>
#endif
#define CELLSIZE 10000000
#define HEAPSIZE  7000000
#define FREESIZE      500
#define STACKSIZE   1000000
#define TRAILSIZE   800000
#define CTRLSTKSIZE  1000
#define VARIANTSIZE  5000000
#define VARIANTMAX  15000000
#define ARITY 17
#define ATOMSIZE 256
#define BUFSIZE 256
#define STRSIZE 256
#define MODULES 10
#define OPERATOR_NUMBER 24
#define FUNCTION_NUMBER 19
#define BUILTIN_NUMBER 190
#define COMPILED_NUMBER 23
#define EXTENDED_NUMBER  12
#define NIL     0
#define YES     2
#define NO      4
#define FEND    6
#define UNDEF   8
#define CUT     10
#define AND     12
#define OR      14
#define LEFTPAREN   16
#define RIGHTPAREN  18
#define CALL    20
#define QUEST	22
#define ANOYVAR 24
#define DEFINE  26
#define ATMARK  28
#define COLON   30
#define OPLTRUE 32
#define OPLFALSE    34
#define CURL    36
#define IFTHEN  38
#define NUMVAR  40
#define UNDERBAR    42
#define DOTOBJ  44
#define HASHTBSIZE 107
#define BIGNUM_BASE 1000000000
#define SMALL_INT_MAX       1000000000
#define SMALL_INT_MIN       -1000000000
#define UNBIND              -1000000001
#define INT_FLAG    1073741824 //#b1000000000000000000000000000000
#define INT_MASK    1073741823 //#b0111111111111111111111111111111
#define PI      3.141592653589793
#define LESS    0
#define NOTLESS 1
//following are for unicode<=>UTF-8 transform
#define UNI2ADD1    192        //#b11000000
#define UNI3ADD1    224        //#b11100000
#define UNI4ADD1    240        //#b11110000
#define UNIOADDO    128        //#b10000000
#define UNI2MSK1    1984       //#b0000011111000000
#define UNI2MSK2    63         //#b0000000000111111
#define UNI3MSK1    61440      //#b1111000000000000
#define UNI3MSK2    4032       //#b0000111111000000
#define UNI3MSK3    63         //#b0000000000111111
#define UNI4MSK1    1835008    //#b00000000000111000000000000000000
#define UNI4MSK2    258048     //#b00000000000000111111000000000000
#define UNI4MSK3    4032       //#b00000000000000000000111111000000
#define UNI4MSK4    63         //#b00000000000000000000000000111111
#define UTF2MSK1    63         //#b00111111
#define UTF3MSK1    31         //#b00011111
#define UTF4MSK1    15         //#b00001111
#define UTFOMSKO    127        //#b01111111
// following are data for SJIS transform
#define SJIS1       65280      //#b1111111100000000
#define SJIS2       255        //#b0000000011111111


typedef enum tag {EMP,INTN,FLTN,LONGN,BIGX,STRUCT,SINGLE,STREAM,STR} tag;
typedef enum flag {FRE,USE} flag;

typedef struct{
    union{
        double fltnum;
        long long int lngnum;
        struct {
            union{
                int intnum;
                int     ( *subr) (int, int);
                FILE    *port;
                int     *dyna_vec;
            } car;
            union{
                int intnum;
            } cdr;
        };
    } val;
    int     aux;
    int     var;
    int     attr;
    int     arity;
    char    tag;
    flag    flag;
    char    *name;
    unsigned char option;
    unsigned char option1;
    unsigned char trace;
    unsigned char length;
} cell;


typedef enum toktype {LPAREN,RPAREN,LBRACKET,RBRACKET,VERTICAL,LCURL,RCURL,
                      STRING,INTEGER,FLOATN,CHARCODE,ATOMOBJ,BUILTIN,
                      COMPILED,OPERATOR,VARIABLE,ANOYMOUS,QUOTE,DOT,BACKQUOTE,
                      COMMA,SEMICOLON,BIGNUM,BINNUM,OCTNUM,HEXNUM,
                      PERIOD,FILEEND,OTHER} toktype;
typedef enum backtrack {GO,BACK} backtrack;
typedef enum spaceskip {SKIP,NOTSKIP} spaceskip;

typedef struct token {
    char ch;
    char ahead;
    backtrack flag;
    spaceskip space;
    toktype type;
    char buf[BUFSIZE];
} token;

typedef void (*p_cut1)(void);

extern cell heap[CELLSIZE];
extern int variant[VARIANTSIZE][2];
extern int stack[STACKSIZE];
extern int goal[ARITY];
extern int head[ARITY];
extern int trail[TRAILSIZE][ARITY];
extern int store[TRAILSIZE][ARITY];
extern int store1[100][ARITY][10];
extern int store_pt;
extern int numbervars[20][3];
extern int numbervars_base_pt;
extern int numbervars_top_pt;
extern token stok;
extern jmp_buf buf;
extern jmp_buf catch_buf[CTRLSTKSIZE];
extern int catch_dt[CTRLSTKSIZE][5];
extern int catch_pt;
extern int cell_hash_table[HASHTBSIZE];
extern int variables;
extern int predicates;
extern int spy_list;
extern int findall_list[10];
extern int findall_pt;
extern int bagof_list;
extern int checked_var;
extern int checking_var;
extern int load_list;
extern int dynamic_list;
extern int contiguous_list;
extern int discontiguous_list;
extern int reconsult_list;
extern int init_list;
extern int op_list;
extern int flag_list;
extern int define_list;
extern int include_list;
extern int option_list;
extern int global_list;
extern int execute_list;
extern int proof;
extern int parse_mode;
extern int line;
extern int column;
extern int trail_end;
extern double micro_second;
extern p_cut1 cut_function[2];
extern int module[10][2];
extern int module_pt;
extern int module_name;
extern int export_list;
extern int meta_list;
extern int context;
extern int cleanup_dt;
extern int numbervars_pt;
extern int unread;
extern int paren_nest;
extern char operator[OPERATOR_NUMBER][5];
extern char function[FUNCTION_NUMBER][12];
extern char builtin[BUILTIN_NUMBER][30];
extern char compiled[COMPILED_NUMBER][30];
extern char extended[EXTENDED_NUMBER][30];

//stream
extern int standard_input;
extern int standard_output;
extern int standard_error;
extern int input_stream;
extern int output_stream;
extern int error_stream;

//stream type
#define OPL_OPEN    21
#define OPL_INPUT   22
#define OPL_OUTPUT  23
#define OPL_TEXT    24
#define OPL_BINARY  25

//trace mode
#define OFF     0
#define FULL    1
#define SPY     2


#define DEBUG               printf("debug\n"); longjmp(buf,2);
#define GET_FLT(addr)       heap[addr].val.fltnum
#define GET_CAR(addr)       heap[addr].val.car.intnum
#define GET_CDR(addr)       heap[addr].val.cdr.intnum
#define GET_AUX(addr)       heap[addr].aux
#define GET_VAR(addr)       heap[addr].var
#define GET_ATTR(addr)      heap[addr].attr
#define GET_ARITY(addr)     heap[addr].arity
#define GET_SUBR(addr)      heap[addr].val.car.subr
#define GET_PORT(addr)      heap[addr].val.car.port
#define GET_INT(addr)       get_int(addr)
#define GET_NUMBER(addr)    heap[addr].val.car.intnum
#define GET_FLT(addr)       heap[addr].val.fltnum
#define GET_LONG(addr)      heap[addr].val.lngnum
#define GET_NAME(addr)      heap[addr].name
#define GET_NAME_ELT(addr,n)    heap[addr].name[n]
#define GET_CHAR(addr)      heap[addr].name[0]
#define GET_TR(addr)        heap[addr].trace
#define GET_TAG(addr)       get_tag(addr)
#define GET_CAR(addr)       heap[addr].val.car.intnum
#define GET_OPT(addr)       heap[addr].option
#define GET_OPT1(addr)       heap[addr].option1
#define GET_LENGTH(addr)    heap[addr].length
#define GET_FLAG(addr)      heap[addr].flag
#define SET_TAG(addr,x)     heap[addr].tag = x
#define SET_CAR(addr,x)     heap[addr].val.car.intnum = x
#define SET_CDR(addr,x)     heap[addr].val.cdr.intnum = x
#define SET_AUX(addr,x)     heap[addr].aux = x
#define SET_VAR(addr,x)     heap[addr].var = x
#define SET_ATTR(addr,x)    heap[addr].attr = x
#define SET_ARITY(addr,x)   heap[addr].arity = x
#define SET_INT(addr,x)     heap[addr].val.car.intnum = x
#define SET_FLT(addr,x)     heap[addr].val.fltnum = x
#define SET_LONG(addr,x)    heap[addr].val.lngnum = x
#define SET_SUBR(addr,x)    heap[addr].val.car.subr = x
#define SET_PORT(addr,x)    heap[addr].val.car.port = x
#define SET_OPT(addr,x)     heap[addr].option = x
#define SET_OPT1(addr,x)    heap[addr].option1 = x
#define SET_LENGTH(addr,x)  heap[addr].length = x
#define SET_CHAR(addr,x)    heap[addr].name[0] = x
#define SET_TR(addr,x)      heap[addr].trace = x
#define SET(addr,x)         heap[addr] = heap[x]
#define IS_INCELL(addr)     (addr >= 0 && addr < CELLSIZE)
#define IS_OUTCELL(addr)    (addr < 0 || addr >= CELLSIZE)
#define IS_ALPHA(addr)      (addr < VARIANTMAX && addr > CELLSIZE)
#define IS_SINGLE(addr)     heap[addr].tag == SINGLE
#define IS_BIGXNUM(addr)    heap[addr].tag == BIGX
#define IS_LONGNUM(addr)    heap[addr].tag == LONGN
#define IS_FLOAT(addr)      heap[addr].tag == FLTN
#define IS_STRING(addr)     heap[addr].tag == STR
#define IS_STRUCT(addr)     heap[addr].tag == STRUCT
#define IS_NIL(addr)        (addr == 0)
#define IS_T(addr)          (addr == 2)
#define IS_EMPTY(addr)      heap[addr].tag  == EMP
#define IS_STREAM(addr)     heap[addr].tag == STREAM
#define HAS_NAME(addr,x)    strcmp(heap[addr].name,x) == 0
#define SAME_NAME(addr1,addr2) strcmp(heap[addr1].name, heap[addr2].name) == 0
#define GREATER_NAME(addr1,addr2) strcmp(heap[addr1].name, heap[addr2].name) > 0
#define SMALLER_NAME(addr1,addr2) strcmp(heap[addr1].name, heap[addr2].name) < 0
#define EQUAL_STR(x,y)      strcmp(x,y) == 0
#define EQUAL_STR(x,y)      strcmp(x,y) == 0
#define STRING_REF(addr,k)   heap[addr].name[k]
#define STRING_SET(addr,k,c) heap[addr].name[k] = c
#define MARK_CELL(addr)     heap[addr].flag = USE
#define NOMARK_CELL(addr)   heap[addr].flag = FRE
#define USED_CELL(addr)     heap[addr].flag == USE
#define FREE_CELL(addr)     heap[addr].flag == FRE

#define isUni1(c)   ((unsigned char)(c) <= 0x7f)

#define isUni2(c)   (((unsigned char)(c) >= 0xc2) && \
                     ((unsigned char)(c) <= 0xdf))

#define isUni3(c)   (((unsigned char)(c) >= 0xe0) && \
                     ((unsigned char)(c) <= 0xef))

#define isUni4(c)   (((unsigned char)(c) >= 0xf0) && \
                     ((unsigned char)(c) <= 0xf7))

#define isUni5(c)   (((unsigned char)(c) >= 0xf8) && \
                     ((unsigned char)(c) <= 0xfb))

#define isUni6(c)   (((unsigned char)(c) >= 0xfc) && \
                     ((unsigned char)(c) <= 0xfd))


#define iskanji(c)  (((unsigned char)(c) >= 0x81 && \
                      (unsigned char)(c) <= 0x9f) || \
                     ((unsigned char)(c) >= 0xe0 && \
                      (unsigned char)(c) <= 0xfc))
#define iskanji2(c)  ((unsigned char)(c) >= 0x40 && \
                      (unsigned char)(c) <= 0xfc && \
                      (unsigned char)(c) != 0x7f)



//operator
#define FX  1   //0b0000001
#define FY  2   //0b0000010
#define XFX 4   //0b0000100
#define XFY 8   //0b0001000
#define YFX 16  //0b0010000
#define XF  32  //0b0100000
#define YF  64  //0b1000000
#define FX_XFX  5 //0b0000101
#define FY_XFX  6 //0b0000110
#define FX_XFY  9 //0b0001001
#define FY_XFY 10 //0b0001010
#define FX_YFX 17 //0b0010001
#define FY_YFX 18 //0b0010010
#define FX_XF  33 //0b0100001
#define FX_YF  65 //0b1000001
#define FY_XF  34 //0b0100010
#define FY_YF  66 //0b1000010

//clause option
#define HASCUT  100 //the clause has cut operator
#define CUTING  101 //the clause that has cut is now on executing.
#define HASLOOP 102 //the clause has simple loop
#define HASIFTHEN 103 // the clause has ih_then ->

//atom type
#define SIMP    1 //simple atom(constant)
#define VAR     2 //variable
#define ANOY    3 //anoimouse
#define USER    4 //user defined operator
#define OPE     5 //operator
#define PRED    6 //user defined predicate
#define SYS     7 //system predicate
#define CLAUSE  8 //clause
#define COMP    9 //compiled predicate
#define LIST    10 //list


//stream type
#define NORMAL  0
#define STDIO   1 //user_input user_output error

//------flag---
extern int trace_flag;
extern int open_flag;
extern int gbc_flag;
extern int simp_flag;
extern int assert_flag;
extern int mode_flag;
extern int debug_flag;
extern int sexp_flag;
extern int quoted_flag;
extern int ignore_flag;
extern int numbervars_flag;
extern int undefined_flag;
extern int double_flag;
extern int link_flag;
extern int rounding_flag;
extern int cut_flag;
extern int listing_flag;
extern int colon_sets_calling_context_flag;
extern int prefix_flag;
extern double time_flag;

//------pointer----
extern int hp; //heap pointer
extern int sp; //stack pointer
extern int fc; //free counter
extern int tp; //trail stack pointer
extern int ac; //alpha conversion variable counter
extern int cp; //cut point
extern int tp; //trail stack pointer
extern int wp; //working pointer

#if defined( __linux) || defined(__OpenBSD__)
//-----editor-----
extern int repl_flag;
extern int buffer[256][10];
extern int ed_row;
extern int ed_col;
extern int ed_start;
extern int ed_end;
extern int ed_ins;
extern int ed_tab;
extern int ed_indent;
extern int ed_name;
extern char ed_data[1000][80];
extern char ed_copy[500][80];
extern int ed_lparen_row;
extern int ed_lparen_col;
extern int ed_rparen_row;
extern int ed_rparen_col;
extern int ed_lbracket_row;
extern int ed_lbracket_col;
extern int ed_rbracket_row;
extern int ed_rbracket_col;
extern int ed_clip_start;
extern int ed_clip_end;
extern int ed_copy_end;
extern char ed_candidate[15][30];
extern int ed_candidate_pt;
extern int ed_operator_color;
extern int ed_builtin_color;
extern int ed_extended_color;
extern int ed_quote_color;
extern int ed_comment_color;
extern int ed_function_color;
extern int ed_incomment;

#define ESCHOME printf("\33[1;1H")
#define ESCTOP  printf("\33[2;1H")
#define ESCCLS  printf("\33[2J")
#define ESCCLS1 printf("\33[0J")
#define ESCCLSL printf("\33[0K")
#define ESCMVLEFT(x) printf("\33[%dG", x)
#define ESCMVR  printf("\33[1C")
#define ESCMVL  printf("\33[1D")
#define ESCMVU  printf("\33[1A")
#define ESCMVD  printf("\33[1B")
#define ESCSCR  printf("\33[S")
#define ESCMOVE(x,y)    printf("\33[%d;%df", x,y)
#define ESCFBLACK printf("\33[30m")
#define ESCFRED  printf("\33[31m")
#define ESCFGREEN printf("\33[32m")
#define ESCFYELLOW printf("\33[33m")
#define ESCFBLUE printf("\33[34m")
#define ESCFMAGENTA printf("\33[35m")
#define ESCFCYAN printf("\33[36m")
#define ESCFWHITE printf("\33[37m")
#define ESCFORG  printf("\33[39m")
#define ESCBCYAN printf("\33[46m")
#define ESCBORG  printf("\33[49m")
#define ESCREV  printf("\33[7m")
#define ESCRST  printf("\33[0m")
#define ESCBOLD printf("\33[1m")

#endif


//-------read--------
#define EOL     '\n'
#define TAB     '\t'
#define SPACE   ' '
#define ESCAPE  033
#define NUL     '\0'
#define ESC     27
#define NUL     '\0'
#define BEL     '\a'
#define BS      '\b'
#define DEL     127
#define FF      12
#define CR      13
#define VT      11

#if defined(__linux) || defined(__OpenBSD__)
#define LEFT    'D'
#define UP      'A'
#define RIGHT   'C'
#define DOWN    'B'
#define INSERT  '2'
#define DELETE  '3'
#define PAGEUP  '5'
#define PAGEDN  '6'
#define HOME    'H'
#define END     'F'
#endif

//-------error code---
#define SYNTAX_ERR      1
#define BUILTIN_EXIST   2
#define CANT_READ       3
#define NOT_COMPUTABLE  9
#define OUT_OF_RANGE    10
#define MALLOC_OVERF    11
#define WRONG_ARGS      12
#define NOT_NUM         13
#define NOT_STR         14
#define NOT_LIST        15
#define NOT_ATOM         16
#define ILLEGAL_OPL_INPUT   17
#define UNCAUGHT_EXCEPTION       19
#define CANT_OPEN       20
#define ILLEGAL_ARGS    21
#define NOT_CONS        22
#define CANT_MODIFY     23
#define NOT_INT         24
#define NOT_STREAM      25
#define NOT_OUT_STREAM  26
#define NOT_IN_STREAM   27
#define NOT_CHAR        28
#define NOT_FLT         29
#define CTRL_OVERF      30
#define END_STREAM      31
#define DIV_ZERO        32
#define CANT_PARSE      33
#define NOT_ARITHMETIC  34
#define FLT_OVERF       36
#define FLT_UNDERF      38
#define STACK_OVERF     41
#define TRAIL_OVERF     42
#define SYSTEM_ERROR    44
#define TCPIP           45
#define UNDEF_PRED		46
#define DISCONTIGUOUS   47
#define NOT_EXIST_MODULE 48
#define EOF_ERROR       49
#define INSTANTATION_ERR    50
#define EXPONENT_ERR    51
#define OPE_SPEC_ERR    52
#define NOT_CALLABLE    53
#define NOT_VAR         54
#define EXISTENCE_ERR   55
#define NOT_SOURCE      56
#define ALIAS_EXIST     57
#define NOT_IO_MODE     59
#define NOT_CLOSE_OPTION    60
#define NOT_STREAM_OPTION   61
#define NOT_OUTPUT_STREAM   62
#define NOT_ATOMIC          63
#define NOT_LESS_THAN_ZERO  64
#define NOT_COMPOUND        65
#define NON_EMPTY_LIST      66
#define NOT_INPUT_STREAM    67
#define PAST_EOF_INPUT      68
#define EVALUATION_ERR      69
#define STATIC_PROCEDURE    70
#define PRED_INDICATOR      71
#define NOT_WRITE_OPTION    72
#define OPE_PRIORITY_ERR    73
#define MODIFY_OPE_ERR      74
#define NOT_READ_OPTION     75
#define NOT_CHAR_CODE       76
#define NOT_FLAG_SPEC       77
#define RESOURCE_ERR        78
#define NOT_ORDER           79

double getETime(void);
#if _WIN32
wint_t readc(void);
#elif __linux
int readc(void);
#elif __OpenBSD__
int readc(void);
#endif
int absolute(int x);
int add_atom_pred_prefix(int pred);
int add_prefix(int x);
int add_body_prefix(int body);
int add_body_prefix1(int x);
int add_pred_prefix(int pred);
int addatom(char *name, int property, int index);
int addtail(int x, int y);
int addtail_operation(int x, int y);
int after_cut(int x);
int after_c_lang(int x);
int alias_option_p(int x);
int aliasp(int addr);
int alpha_conversion(int x);
int alpha_variable_p(int addr);
int alpha_to_variable(int x);
int alphabeticalp(int addr);
int already_checked_p(int x);
int angle(int y, int x);
int anoymousp(int x);
int anoymous_conversion(int x);
int append(int x, int y);
int append1(int x, int y);
int argumentsp(int addr);
int assq(int sym, int lis);
int assq1(int sym, int lis);
int atmarkp(int addr);
int atom_chars_list_p(int addr);
int atom_codes_list_p(int addr);
int atom_constant_p(int addr);
int atom_length(int addr);
int atom_predicate_p(int addr);
int atom_quote_p(int addr);
int atom_variable_p(int addr);
int atomp(int addr);
int atomicp(int addr);
int atsmaller(int x, int y);
int ateqsmaller(int x, int y);
int b_abolish(int nest, int n);
int b_abort(int nest, int n);
int b_append(int nest, int n);
int b_arg(int nest, int n);
int b_argument_list(int nest, int n);
int b_arity_count(int nest, int n);
int b_ask(int nest, int n);
int b_assert(int nest, int n);
int b_asserta(int nest, int n);
int b_at_end_of_stream(int nest, int n);
int b_ateqgreater(int nest, int n);
int b_ateqsmaller(int nest, int n);
int b_atgreater(int nest, int n);
int b_atmark(int nest, int n);
int b_atom(int nest, int n);
int b_atom_chars(int nest, int n);
int b_atom_codes(int nest, int n);
int b_atom_concat(int nest, int n);
int b_atom_length(int nest, int n);
int b_atomic(int nest, int n);
int b_atom_convert(int nest, int n);
int b_atsmaller(int nest, int n);
int b_bagof(int nest, int n);
int b_bagof_help(int nset, int n);
int b_between(int nest, int n);
int b_bignum(int nest, int n);
int b_body(int nest, int n);
int b_call(int nest, int n);
int b_callable(int nest, int n);
int b_calling_context(int nest, int n);
int b_change_directory(int nest , int n);
int b_catch(int nest, int n);
int b_char_code(int nest, int n);
int b_char_conversion(int nest, int n);
int b_char_set(int nest, int n);
int b_clause(int nest, int n);
int b_close(int nest, int n);
int b_colon(int nest, int n);
int b_compare(int nest, int n);
int b_compound(int nest, int n);
int b_consult(int nest, int n);
int b_constant(int nest, int n);
int b_copy_term(int nest, int n);
int b_current_directory(int nest , int n);
int b_current_input(int nest, int n);
int b_current_output(int nest, int n);
int b_current_op(int nest, int n);
int b_current_predicate(int nest, int n);
int b_current_prolog_flag(int nest, int n);
int b_current_module(int nest, int n);
int b_current_visible(int nest, int n);
int b_cut(int nest, int n);
int b_comp_ifthen(int nest, int n);
int b_comp_ifthenelse(int nest, int n);
int b_debug(int nest, int n);
int b_decompose(int nest, int n);
int b_decompose_file_name(int nest, int n);
int b_defined_predicate(int nest, int n);
int b_defined_userop(int nest, int n);
int b_delete_file(int nest, int n);
int b_directory_exists(int nest, int n);
int b_dynamic(int nest, int n);
int b_dumpcell(int nest, int n);
int b_discontiguous(int nest, int n);
int b_initialization(int nest, int n);
int b_end_body(int nest, int n);
int b_end_module(int nest, int n);
int b_environment_variable(int nest, int n);
int b_ensure_loaded(int nest, int n);
int b_eqgreater(int nest, int n);
int b_eqsmaller(int nest, int n);
int b_equalp(int nest, int n);
int b_expand_path(int nest, int n);
int b_export(int nest, int n);
int b_fail(int nest, int n);
int b_fail_if(int nest, int n);
int b_file_exists(int nest, int n);
int b_file_modification_time(int nest, int n);
int b_findall(int nest, int n);
int b_findall_help(int nest ,int n);
int b_findatom(int nest, int n);
int b_filename(int nest, int n);
int b_flush_output(int nest, int n);
int b_freeze(int nest, int n);
int b_functor(int nest, int n);
int b_gbc(int nest, int n);
int b_get(int nest, int n);
int b_get0(int nest, int n);
int b_get_byte(int nest, int n);
int b_get_char(int nest, int n);
int b_get_code(int nest, int n);
int b_greater(int nest, int n);
int b_generate_variable(int nest, int n);
int b_generate_all_variable(int nest, int n);
int b_generate_before_cut(int nest, int n);
int b_generate_after_cut(int nest, int n);
int b_generate_before_c_lang(int nest, int n);
int b_generate_after_c_lang(int nest, int n);
int b_generated_module(int nest, int n);
int b_ground(int nest, int n);
int b_halt(int nest, int n);
int b_has_cut(int nest, int n);
int b_has_c_lang(int nest, int n);
int b_ifthen(int nest, int n);
int b_import(int nest, int n);
int b_include(int nest, int n);
int b_include_cut(int nest, int n);
int b_integer(int nest, int n);
int b_is(int nest, int n);
int b_isp(int nest, int n);
int b_keysort(int nest, int n);
int b_length(int nest, int n);
int b_longnum(int nest, int n);
int b_list(int nest, int n);
int b_listing(int nest, int n);
int b_make_directory(int nest, int n);
int b_maplist(int nest, int n);
int b_max_arity(int nest, int n);
int b_member(int nest, int n);
int b_meta(int nest, int n);
int b_module(int nest, int n);
int b_multifile(int nest, int n);
int b_nano(int nest, int n);
int b_nl(int nest, int n);
int b_nonvar(int nest, int n);
int b_nospy(int nest, int n);
int b_notequalp(int nest, int n);
int b_notnumeq(int nest, int n);
int b_notrace(int nest, int n);
int b_notunify(int nest, int n);
int b_number(int nest, int n);
int b_number_chars(int nest, int n);
int b_number_codes(int nest, int n);
int b_numbervars(int nest, int n);
int b_numeq(int nest, int n);
int b_o_c_define(int nest, int n);
int b_o_c_include(int nest, int n);
int b_o_c_option(int nest, int n);
int b_o_c_global(int nest, int n);
int b_once(int nest, int n);
int b_op(int nest, int n);
int b_open(int nest, int n);
int b_peek_byte(int nest, int n);
int b_predicate_property(int nest, int n);
int b_property(int nest, int n);
int b_put(int nest, int n);
int b_put_char(int nest, int n);
int b_put_code(int nest, int n);
int b_put_byte(int nest, int n);
int b_read(int nest, int n);
int b_read_item(int nest, int n);
int b_read_term(int nest, int n);
int b_read_token(int nest, int n);
int b_real(int nest, int n);
int b_reconsult(int nest, int n);
int b_reconsult_predicate(int nest, int n);
int b_reconsult_predicate_list(int nest, int n);
int b_repeat(int nest, int n);
int b_retract(int nest, int n);
int b_retractall(int nest, int n);
int b_reverse(int nest, int n);
int b_integer_rounding_function(int nest, int n);
int b_see(int nest, int n);
int b_seen(int nest, int n);
int b_select(int nest, int n);
int b_setof(int nest, int n);
int b_set_input(int nest, int n);
int b_set_prolog_flag(int nest, int n);
int b_set_output(int nest, int n);
int b_set_matvec(int nest, int n);
int b_setup_call_cleanup(int nest, int n);
int b_smaller(int nest, int n);
int b_sort(int nest, int n);
int b_spy(int nest, int n);
int b_store(int nest, int n);
int b_stream_property(int nest, int n);
int b_string(int nest, int n);
int b_string_chars(int nest, int n);
int b_string_codes(int nest, int n);
int b_sub_atom(int nest, int n);
int b_succ(int nest, int n);
int b_tab(int nest, int n);
int b_tell(int nest, int n);
int b_term_variables(int nest, int n);
int b_throw(int nest, int n);
int b_time(int nest, int n);
int b_timer(int nest, int n);
int b_told(int nest, int n);
int b_trace(int nest, int n);
int b_true(int nest, int n);
int b_undefined_predicate(int nest, int n);
int b_unify(int nest, int n);
int b_unify_with_occurs_check(int nest, int n);
int b_univ(int nest, int n);
int b_use_module(int nest, int n);
int b_var(int nest, int n);
int b_variable_convert(int nest, int n);
int b_write(int nest, int n);
int b_writeln(int nest, int n);
int b_write_term(int nest, int n);
int b_write_canonical(int nest, int n);
int b_writeq(int nest, int n);
int before_cut(int x);
int before_cut1(int x, int y);
int before_c_lang(int x);
int bignump(int x);
int bigx_abs(int x);
int bigx_abs_smallerp(int arg1, int arg2);
int bigx_big_to_flt(int x);
int bigx_eqp(int x, int y);
int bigx_greaterp(int arg1, int arg2);
int bigx_int_to_big(int x);
int bigx_length(int x);
int bigx_long_to_big(int x);
int bigx_minus(int arg1, int arg2);
int bigx_minus1(int arg1, int arg2);
int bigx_mult(int arg1, int arg2);
int bigx_mult_i(int x, int y);
int bigx_mult1(int arg1, int arg2);
int bigx_negativep(int x);
int bigx_plus(int arg1, int arg2);
int bigx_plus1(int arg1, int arg2);
int bigx_positivep(int x);
int bigx_quotient(int arg1, int arg2);
int bigx_quotient_i(int x, int y);
int bigx_quotient1(int arg1, int arg2);
int bigx_remainder_i(int x, int y);
int bigx_shift(int x, int n);
int bigx_simplify(int x);
int bigx_smallerp(int arg1, int arg2);
int builtin_zero_p(int addr);
int builtinp(int addr);
int butlast(int addr);
int callsubr(int x,int nest,int n);
int caar(int addr);
int cadar(int addr);
int cadddr(int addr);
int caddr(int addr);
int cadr(int addr);
int car(int addr);
int callablep(int addr);
int cdar(int addr);
int cddr(int addr);
int cdr(int addr);
int characterp(int addr);
int clausep(int addr);
int colonp(int addr);
int compiler_variable_p(int x);
int compiled_zero_p(int addr);
int compiledp(int addr);
int compoundp(int addr);
int concat_atom(int x, int y);
int conjunctionp(int addr);
int cons(int car, int cdr);
int cons_next(int x, int y);
int cons_prev(int x, int y);
int constantp(int addr);
int copy_heap(int x);
int copy_term(int x);
int copy_variable(int x);
int ctrl_to_number(char c);
int c_lang_p(int x);
int deref(int x);
int deref1(int x);
int deref_array(int arity[256], int head);
int disjunctionp(int addr);
int divide(int arg1, int arg2);
int eof_action_option_p(int x);
int eqgreaterp(int x1, int x2);
int eqlp(int addr1, int addr2);
int eqp(int addr1, int addr2);
int eqsmallerp(int x1, int x2);
int equalp(int addr1, int addr2);
int eval(int x);
void evalterm(int x, int [3]);
int exact_to_inexact(int x);
int exist_unbind_var_p(int x);
int export_check(int pred);
int expt(int x, int y);
int f_abs(int x);
int f_div(int x, int y);
int f_divide(int x, int y);
int f_exclusiveor(int x, int y);
int f_expt(int x, int y);
int f_inclusiveand(int x, int y);
int f_leftshift(int x, int y);
int f_logicaland(int x, int y);
int f_logicalor(int x, int y);
int f_complement(int x, int y);
int f_minus(int x, int y);
int f_mod(int x, int y);
int f_rem(int x, int y);
int f_mult(int x, int y);
int f_plus(int x, int y);
int f_rightshift(int x, int y);
int f_round(int x);
int f_sqrt(int x);
int f_sin(int x);
int f_asin(int x);
int f_cos(int x);
int f_acos(int x);
int f_tan(int x);
int f_atan(int x);
int f_floor(int x);
int f_ceiling(int x);
int f_truncate(int x);
int f_sign(int x);
int f_exp(int x);
int f_log(int x);
int f_float(int x);
int f_float_integer_part(int x);
int f_float_fraction_part(int x);
int f_min(int x, int y);
int f_max(int x, int y);
int f_random(int x);
int findatom(int x, int property);
int findvar(int sym);
int floatp(int x);
int formulap(int addr);
int formula1p(int addr);
int forcefalsep(int x);
int forcetruep(int x);
int freshcell(void);
int functionp(int addr);
int gcd(int x, int y);
int generate_all_variable(int x);
int generate_variable(int x);
int generate_variable1(int x, int y);
int gen_big(void);
int gen_n(int n);
int get_1st_weight(int addr);
int get_2nd_weight(int addr);
int get_msb(int x);
int get_nth(int x, int n);
int get_sign(int x);
int get_tp(void);
int get_sp(void);
int get_wp(void);
int get_goal(int n);
int get_cut_jmp(void);
int get_trail_end(void);
int get_notfree_variable(int x);
int get_free_variable(int x, int y);
int get_predicate(int x);
int getatom(char *name, int property, int index);
int getsym(char *name, int index);
int greaterp(int x1, int x2);
int groundp(int addr);
int has_cut_p(int addr);
int has_c_lang_p(int addr);
int has_ifthen_p(int addr);
int has_loop_p(int x, int y);
int has_no_value_p(int x);
int has_value_p(int addr);
int has_variable_p(int addr);
int hash(char *name);
int heavy999p(int addr);
int last(int x);
int ifthenp(int addr);
int ignore_optin_p(int x);
int infixp(int addr);
int insert(int x, int y);
int int_gcd(int x, int y);
int int_lcm(int m, int n);
int integerp(int addr);
int isatomch(char c);
int isoctch(char c);
int isbinch(char c);
int ishexch(char c);
int isnumlis(int arg);
int isqrt(int x);
int isqrt1(int s, int s2, int x);
int keysort(int x);
int keyinsert(int x, int y);
int last_predicate(int x);
int laststr(char buf[]);
int lcm(int x, int y);
int length(int addr);
int list(int addr);
int list_to_ope(int x);
int list_to_structure(int x);
int list1(int x);
int list2(int x, int y);
int list3(int x1, int x2, int x3);
int list4(int x1, int x2, int x3, int x4);
int list5(int x1, int x2, int x3, int x4, int x5);
int list6(int x1, int x2, int x3, int x4, int x5, int x6);
int list7(int x1, int x2, int x3, int x4, int x5, int x6, int x7);
int list8(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8);
int list9(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9);
int list10(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10);
int listp(int addr);
int listremove(int x, int y);
int listremove3(int x1, int x2, int x3, int ls);
int listreverse(int x);
int aritycheck(int x, int y);
int long_int_quotient(int x, int y);
int long_int_remainder(int x, int y);
int long_long_quotient(int x, int y);
int long_long_remainder(int x, int y);
int longnump(int x);
int makealias(char *name, int stream, int type);
int makeatom(char *name, int property);
int makeatom1(char *name, int property);
int makeconst(char *name);
int makebigx(char *bignum);
int makehexbigx(char *bignum);
int makeoctbigx(char *bignum);
int makeope(char *name);
int makebinbigx(char *bignum);
int makecomp(char *name);
int makecopy(int x);
int makeexspec(int old_spec, int spec);
int makeflt(double floatn);
int makeint(int num);
int makelong(long long int lngnum);
int makepred(char *name);
int makespec(int spec);
int makestr(char *name);
int makesys(char *name);
int makestream(FILE *port, int i_o, int type, int action ,int fname);
int makeuser(char *name);
int makevariant(void);
int makevar(char *name);
int memory_variant(int x);
int memq(int x, int y);
int memberp(int x, int y);
int meta_check(int pred, int arity);
int minus(int arg1, int arg2);
int mixturep(int addr);
int mult(int arg1, int arg2);
int mode_option_p(int x);
int negative_zerop(int x);
int negativep(int x);
int next(int x);
int nfindvar(int x);
int notated_builtinp(int addr);
int not_numeqp(int x, int y);
int now_checking_var_p(int x);
int nth(int addr, int n);
int nullp(int addr);
int numberp(int addr);
int numbertoken(char buf[]);
int numbervars_option_p(int x);
int numbervarp(int addr);
int numeqp(int x, int y);
int occursp(int x, int y);
int occursp1(int x, int y);
int o_cons(int x, int y);
int o_dcg(int x, int y);
int o_define(int x, int y);
int o_ignore(int nest, int n);
int o_question(int x, int y);
int op_connect(int x, int y);
int infix_operator_p(int addr);
int operate(int x);
int operationp(int addr);
int operatorp(int addr);
int parser(int operand, int operator, int weight, int spec, int terminal, int parsemode);
int parse(int operand, int operator);
int parse1(int operand, int operator ,int weight, int spec);
int plus(int arg1, int arg2);
int pop_stack(void);
int pop_trail(void);
int position_option_p(int x);
int positive_zerop(int x);
int positivep(int x);
int postfixp(int addr);
int predicatep(int addr);
int prefixp(int addr);
int prev(int x);
int proceed(int x, int y);
int quoted_option_p(int x);
int quotient(int x, int y);
int readparse(void);
int readitem(void);
int readcurl(void);
int readlist(void);
int readparen(void);
int remove_cut(int x);
int replace(int x, int lis);
int reposition_option_p(int x);
int resolve_all(int end, int bindings, int n);
int resolve(int end, int bindings, int trail, int n);
int reverse(int x);
int set_goal(int n, int x);
int set_tp(int x);
int set_sp(int x);
int set_wp(int x);
int set_aux(int x, int y);
int set_trail_end(int x);
int set_var(int x, int y);
int s_remainder(int x, int y);
int see_trail(void);
int singletonp(int x);
int singleton_list(void);
int single_operation_p(int x);
int sjis_to_code(char *p);
int smallerp(int x1, int x2);
int sort(int x);
int sortsmaller(int x, int y);
int sorteqlp(int x, int y);
int structure_to_list(int x);
int structurep(int addr);
int streamp(int addr);
int stringp(int addr);
int singlep(int addr);
int streqp(int x, int y);
int symboltoken(char buf[]);
int term_variables(int x, int y);
int type_option_p(int x);
int thunkp(int addr);
int unicodep(char c);
int unify(int x, int y);
int unify_array(int arity[256], int head);
int unify_goal(void);
int unify_head(void);
int unique(int x);
int user_operation_p(int addr);
int user_operator_p(int addr);
int utf8_to_ucs4(char *p);
int valslist(int x);
int variablep(int addr);
int variable_convert1(int x);
int variable_convert2(int x);
int variable_convert3(int x);
int variable_convert4(int x);
int variable_name_list(void);
int variable_name_p(int x);
int variable_to_call(int x);
int varslist(int x);
int walpha_conversion(int x);
int wcons(int car, int cdr);
int wide_integer_p(int addr);
int wide_variable_p(int addr);
int wlistcons(int x, int y);
int wlist1(int x);
int wlist2(int x, int y);
int wlist3(int x1, int x2, int x3);
int wlist4(int x1, int x2, int x3, int x4);
int wlist5(int x1, int x2, int x3, int x4, int x5);
int wlist6(int x1, int x2, int x3, int x4, int x5, int x6);
int wlist7(int x1, int x2, int x3, int x4, int x5, int x6, int x7);
int wlist8(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8);
int wlist9(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9);
int wlist10(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10);
int zerop(int x);
void add_data(int pred, int data);
void assign_variant(int x);
void bigx_gbc(int x);
void bigx_minus2(int arg, int c, int msb);
void bigx_plus2(int arg, int c, int msb);
void bindsym(int x, int val);
void cellprint(int addr);
void checkarg(int test, char *fun, int arg);
void checkgbc(void);
void clrcell(int addr);
void cut_zero(int x);
void debugger(int end, int bindings, int choice, int n);
void defbuiltin(char *name, int(*func)(int, int));
void defcompiled(char *name, int(*func)(int, int));
void definfix(char *name, int(*func)(int, int), int weight, int spec);
void defoperator(char *name, int(*func)(int, int), int weight, int spec, int opt);
void defuserfunction(char *name, int weight, int spec);
void discard_trail(void);
void discard_trail_n(int n);
void error(int errnum, char *fun, int arg);
void execute(int x);
void gbc(void);
void gbcmark(void);
void gbcsweep(void);
void gettoken(void);
void heapdump(int start, int end);
void init_repl(void);
void initbuiltin(void);
void initcell(void);
void initoperator(void);
void initstream(void);
void insert_data(int pred, int data);
void markcell(int addr);
void markoblist(void);
void memoize_arity(int clause, int atom);
void print(int addr);
void printanswer(int addr);
void print_bigx(int x);
void printbody(int addr);
void printbody1(int addr);
void printc(char c);
void printclause(int addr);
void printenv(void);
void print_quoted(int addr);
void print_numbervars(int addr);
void print_not_quoted(int addr);
void print_goal(void);
void print_head(void);
void print_stack(void);
void print_trail(int x);
void print_trail_block(int n);
void printarguments(int addr);
void printcurl(int addr);
void printinfix(int addr);
void printlist(int addr);
void printlist_canonical(int addr);
void printlong(int addr);
void printpostfix(int addr);
void printprefix(int addr);
void printsexp(int addr);
void printtuple(int addr);
void push_stack(int x);
void push_trail(int x);
void push_trail1(int x);
void push_trail_body(int x);
void push_trail_body1(int x);
void push_trail_body_with_ask(int x);
void push_trail_body_with_findall_help(int pred, int var);
void push_trail_body_with_bagof_help(int pred, int var, int free);
void recall_variant(int x);
void release_variant(int x);
void retract_goal(int n);
void retract_trail(int n);
void save_trail(int x);
void set_head(int x);
void store_goal(int nest);
void set_length(int x);
void set_sign(int x, int y);
void sjis_to_char(int n , char *p);
void sprint(int addr);
void throw_error(int type, int info);
void ucs4_to_utf8(int n, char *p);
void unbind(int x);
void unreadc(char c);

//JUMP project
int b_system(int nest, int n);
int b_clause_with_arity(int nest, int n);
int b_c_lang(int nest, int n);
int b_c_define(int nest, int n);
int b_c_include(int nest, int n);
int b_c_option(int nest, int n);
int b_c_global(int nest, int n);
int b_compiler_anoymous(int nest, int n);
int b_compiler_variable(int nest, int n);
int b_self_introduction(int nest, int n);
int b_date(int nest, int n);
int b_dynamic_check(int nest, int n);
int b_get_dynamic(int nest, int n);
int b_get_execute(int nest, int n);
int b_write_original_variable(int nest, int n);
int dynamic_check1(int x);
int get_tag(int addr);
int get_int(int addr);
int listcons(int x, int y);
int set_car(int x, int y);
int set_cdr(int x, int y);
int makestrflt(char *str);
int makestrlong(char *str);
void debug(void);

#if defined( __linux) || defined(__OpenBSD__)
struct position{
    int row;
    int col;
};

int b_set_editor(int nest, int n);
int b_edit(int nest, int n);
void edit_screen(int x);
void display_command(int arg);
void display_screen();
void display_line(int line);
void setcolor(int n);
void backspace();
void insertcol();
void insertrow();
int getch();
void deleterow();
int findeol(int row);
struct position findlparen(int bias);
struct position findrparen(int bias);
struct position findlbracket(int bias);
struct position findrbracket(int bias);
void restore_paren();
void restore_bracket();
void emphasis_lparen();
void emphasis_rparen();
void emphasis_lbracket();
void emphasis_rbracket();
void softtabs(int n);
void save_data(int arg1);
int findnext(int row, int col);
void remove_headspace(int row);
void copy_selection();
void paste_selection();
void delete_selection();
int check_token(int row, int col);
char* get_fragment();
void find_candidate();
void replace_fragment(char* newstr);
void display_buffer(void);
int check_token_buffer(int col);
int findlparen_buffer(int col);
int findrparen_buffer(int col);
int findlbracket_buffer(int col);
int findrbracket_buffer(int col);
void emphasis_lparen_buffer(int col);
void emphasis_rparen_buffer(int col);
void emphasis_lbracket_buffer(int col);
void emphasis_rbracket_buffer(int col);
void reset_paren_bracket_buffer();
void restore_paren_buffer(int col);
void restore_bracket_buffer(int col);
char *get_fragment_buffer(int col);
void find_candidate_buffer(int col);
int replace_fragment_buffer(char* newstr, int col);
void insertcol_buffer(int col);
void backspace_buffer(int col);
int read_line(int flag);
int count_col(int x);
int count_col_buffer(int x);

int b_server_create(int nest, int n);
int b_server_accept(int nest, int n);
int b_client_connect(int nest, int n);
int b_socket_send(int nest, int n);
int b_socket_recieve(int nest, int n);
int b_socket_close(int nest, int n);
#endif

//-------debug tool------------
void monitor(int x);
void report_atom(int x);
void report_token(void);
