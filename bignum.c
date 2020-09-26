/*
ported from Normal-Scheme
data type  BIGX
two way list. to become easy to access from LSB and MSB.
car=int data of number
cdr  next pointer to MSB
aux  prev pointer to LSB
aux of MSB store NIL.
opt has sign data. plus=1,minus=-1,zero=0.
number of data is absolute.

*/
#include <stdlib.h>
#include <math.h>
#include "npl.h"


int makebigx(char *bignum){ 
    char integer[15];
    int i,j,res,sign,len,msb;
    long long int l,m;
    
    //check sign
    if(bignum[0] == '-')
        sign = -1;
    else
        sign = 1;
    //remove sign
    if(bignum[0] == '-' || bignum[0] == '+'){
        i = laststr(bignum);
        for(j=0;j<i;j++)
            bignum[j] = bignum[j+1];
        
        bignum[j] = NUL;
    }
    //length to check long-integer
    len = 0;
    //generate origin of BIGNUM 
    msb = res = gen_big();
    
    i = laststr(bignum);
    while(i >= 0){
        if(i > 8){
            for(j=8; j>=0; j--){
                integer[j] = bignum[i];
                i--;
            }
            integer[9] = NUL;
            msb = cons_next(atoi(integer),msb);
            len++;
        }
        else{
            integer[i+1] = NUL;
            while(i >= 0){
                integer[i] = bignum[i];
                i--;
            }
            msb = cons_next(atoi(integer),msb);
            len++;
        }
    }
    
    if(len==2){ 
        l = (long long int)GET_CAR(next(res))*BIGNUM_BASE; 
        m = (long long int)GET_CAR(res);
        m = (l+m)*sign;
        SET_TAG(res,LONGN);
        SET_LONG(res,m);
        return(res);
    }
    else{
        SET_TAG(res,BIGX);
        set_sign(res,sign);
        return(res);
    }    
}

int makehexbigx(char *bignum){ 
    int num,i,j;
    
    j = laststr(bignum);
    i = 0;
    num = makeint(0);
    while(i < j){
        switch(bignum[i]){
            case '0':   num = plus(num,makeint(0));break;
            case '1':   num = plus(num,makeint(1));break;
            case '2':   num = plus(num,makeint(2));break;
            case '3':   num = plus(num,makeint(3));break;
            case '4':   num = plus(num,makeint(4));break;
            case '5':   num = plus(num,makeint(5));break;
            case '6':   num = plus(num,makeint(6));break;
            case '7':   num = plus(num,makeint(7));break;
            case '8':   num = plus(num,makeint(8));break;
            case '9':   num = plus(num,makeint(9));break;
            case 'a':   num = plus(num,makeint(10));break;
            case 'b':   num = plus(num,makeint(11));break;
            case 'c':   num = plus(num,makeint(12));break;
            case 'd':   num = plus(num,makeint(13));break;
            case 'e':   num = plus(num,makeint(14));break;
            case 'f':   num = plus(num,makeint(15));break;
            case 'A':   num = plus(num,makeint(11));break;
            case 'B':   num = plus(num,makeint(12));break;
            case 'C':   num = plus(num,makeint(13));break;
            case 'D':   num = plus(num,makeint(14));break;
            case 'E':   num = plus(num,makeint(15));break;
            case 'F':   num = plus(num,makeint(16));break;
        }                
        i++;                                    
        num = mult(num,makeint(16));
    }
    return(num);    
}

int makeoctbigx(char *bignum){ 
    int num,i,j;
    
    j = laststr(bignum);
    i = 0;
    num = makeint(0);
    while(i < j){
        switch(bignum[i]){
            case '0':   num = plus(num,makeint(0));break;
            case '1':   num = plus(num,makeint(1));break;
            case '2':   num = plus(num,makeint(2));break;
            case '3':   num = plus(num,makeint(3));break;
            case '4':   num = plus(num,makeint(4));break;
            case '5':   num = plus(num,makeint(5));break;
            case '6':   num = plus(num,makeint(6));break;
            case '7':   num = plus(num,makeint(7));break;
        }
        i++;
        num = mult(num,makeint(8));
    }
    return(num);    
}

int makebinbigx(char *bignum){ 
    int num,i,j;
    
    j = laststr(bignum);
    i = 0;
    num = makeint(0);
    while(i < j){
        switch(bignum[i]){
            case '0':   num = plus(num,makeint(0));break;
            case '1':   num = plus(num,makeint(1));break;
        }
        i++;
        num = mult(num,makeint(2));
    }
    return(num);    
}

void print_bigx(int x){
    int y;
    
    if(get_sign(x) == -1)
        printf("-");
    y = get_msb(x);
    fprintf(GET_PORT(output_stream),"%d",GET_CAR(y));
    y = prev(y);
    
    do{
        fprintf(GET_PORT(output_stream),"%09d", GET_CAR(y));
        y = prev(y);
    }while(!nullp(y));
}

/*
x=new y=link
if it is first cell, store the cell, else chain a new cell.
*/
int cons_next(int x, int y){
    int res;
    
    if(GET_AUX(y) == -1){
        SET_AUX(y,NIL);
        SET_CAR(y,x);
        res = y;
    }
    else{
        res = freshcell();
        SET_CAR(res,x);
        SET_CDR(y,res);
        SET_AUX(res,y);
        SET_CDR(res,NIL);
    }
    return(res);
}

//x=link y=new
int cons_prev(int x, int y){
    int res;
    
    if(GET_AUX(x) == -1){
        SET_AUX(x,NIL);
        SET_CAR(x,y);
        res = x;
    }
    else{
        res = freshcell();
        SET_CAR(res,y);
        SET_CDR(res,x);
        SET_AUX(x,res);
        SET_AUX(res,NIL);
    }
    return(res);
}
/*
To check first cell, prop=-1.
therefor when compute bignum, if it is first cell, store data the cell.
or else chain cell with cons_next. 

*/
int gen_big(void){
    int res;
    
    res = freshcell();
    SET_CDR(res,NIL);
    SET_AUX(res,-1);
    return(res);
}

//get address of upper digit
int next(int x){    
    return(GET_CDR(x));
}

//get address of lower digit
int prev(int x){
    return(GET_AUX(x)); 
}

//set sign
void set_sign(int x, int y){
    SET_OPT(x,y);
}
//get sign
int get_sign(int x){
    return(GET_OPT(x));
}

int bigx_positivep(int x){
    if(get_sign(x) == 1)
        return(1);
    else
        return(0);
}

int bigx_negativep(int x){
    if(get_sign(x) == -1)
        return(1);
    else
        return(0);
}
/*
count number of bignum cell.
if value is zero, count is zero.
It is reason for matching to division

*/
int bigx_length(int x){
    int len,y;
    
    len = 0;
    y = x;
    do{
        len++;
        y = next(y);
    }while(!nullp(y));
    if(len==1 && GET_CAR(x)==0)
        return(0);
    else
        return(len);
}

//get address of MSB
int get_msb(int x){
    while(!nullp(next(x)))
        x = next(x);
    
    return(x);
}

//garbage collection of unnecessary bignum cell.
void bigx_gbc(int x){
    int addr,addr1;
    
    addr = x;
    do{ addr1 = next(addr);
        clrcell(addr);
        fc++;
        SET_CDR(addr,hp);
        hp = addr;
        addr = addr1;
    }while(!nullp(addr));     
}


int bigx_eqp(int x, int y){
    do{
        if(GET_CAR(x) != GET_CAR(y))
            return(0);
        
        x = next(x);
        y = next(y);
    }while(!nullp(x) && !nullp(y));
    if(nullp(x) && nullp(y))
        return(1);
    else
        return(0);
}

int bigx_greaterp(int arg1, int arg2){
    int l1,l2,a1,a2;
    
    if(bigx_positivep(arg1) && bigx_negativep(arg2))
        return(1);
    if(bigx_negativep(arg1) && bigx_positivep(arg2))
        return(0);
    
    l1 = bigx_length(arg1);
    l2 = bigx_length(arg2);
    

    if(bigx_positivep(arg1) && bigx_positivep(arg2) && l1 > l2)
        return(1);
    else if(bigx_negativep(arg1) && bigx_negativep(arg2) && l1 < l2)
        return(1);
    else if(l1 == l2 && bigx_positivep(arg1) && bigx_positivep(arg2)){
        a1 = get_msb(arg1);
        a2 = get_msb(arg2);
        do{
            if(GET_CAR(a1) > GET_CAR(a2))
                return(1);
            else
                return(0);
            a1 = prev(a1);
            a2 = prev(a2);
        }while(!nullp(a1));
        return(0);
    }
    else if(l1 == l2 && bigx_negativep(arg1) && bigx_negativep(arg2)){
        a1 = get_msb(arg1);
        a2 = get_msb(arg2);
        do{
            if(GET_CAR(a1) < GET_CAR(a2))
                return(1);
            else
                return(0);
            a1 = prev(a1);
            a2 = prev(a2);
        }while(!nullp(a1));
        return(0);
    }   
    else
        return(0);  
}

int bigx_abs_smallerp(int arg1, int arg2){
    int l1,l2,a1,a2;
    
    l1 = bigx_length(arg1);
    l2 = bigx_length(arg2);
    

    if(l1 < l2)
        return(1);
    else if(l1 > l2)
        return(0);
    else {
        a1 = get_msb(arg1); 
        a2 = get_msb(arg2);
        do{
            if(GET_CAR(a1) < GET_CAR(a2))
                return(1);
            if(GET_CAR(a1) > GET_CAR(a2))
                return(0);
                
            a1 = prev(a1);
            a2 = prev(a2);
        }while(!nullp(a1));
        return(0);
    }
        
}


int bigx_smallerp(int arg1, int arg2){ 
    int l1,l2,a1,a2;
    
    if(bigx_positivep(arg1) && bigx_negativep(arg2))
        return(0);
    if(bigx_negativep(arg1) && bigx_positivep(arg2))
        return(1);
    
    l1 = bigx_length(arg1);
    l2 = bigx_length(arg2);

    if(l1 == l2 && bigx_positivep(arg1) && bigx_positivep(arg2)){
        a1 = get_msb(arg1);
        a2 = get_msb(arg2);
        do{
            if(GET_CAR(a1) < GET_CAR(a2)){
                return(1);
            }
            if(GET_CAR(a1) > GET_CAR(a2)){
                return(0);
            }
            a1 = prev(a1);
            a2 = prev(a2);
        }while(!nullp(a1));
        return(0);
    }
    else if(l1 == l2 && bigx_negativep(arg1) && bigx_negativep(arg2)){
        a1 = get_msb(arg1);
        a2 = get_msb(arg2);
        do{
            if(GET_CAR(a1) > GET_CAR(a2))
                return(1);
            if(GET_CAR(a1) < GET_CAR(a2))
                return(0);    
            
            a1 = prev(a1);
            a2 = prev(a2);
        }while(!nullp(a1));
        return(0);
    }   
    if(bigx_positivep(arg1) && bigx_positivep(arg2)){
        if(l1 > l2)
            return(0);
        else if(l1 < l2)
            return(1);
    }
    else if(bigx_negativep(arg1) && bigx_negativep(arg2)){
        if(l1 < l2)
            return(0);
        else if(l1 > l2)
            return(1);
    }
    
    
    return(0);  
}

//find nth cell address
int get_nth(int x, int n){
    int m,msb;
    
   
    m = 1;
    msb = x;
    while(m<=n){
        msb = next(msb);
        m++;
    }
    return(msb);
}


//generate bignum data of n-cells
int gen_n(int n){
    int i,msb,res;
    
    msb = res = gen_big();
    SET_TAG(res,BIGX);
    SET_CAR(res,0);
    SET_AUX(res,NIL);
    for(i=1;i<n;i++)
        msb = cons_next(0,msb);
    
    return(res);
}


/*
cut of head zero cell.
at least keep one cell, because possibility of all-zero. 

*/
void cut_zero(int x){
    int msb,dig;
    
    dig = bigx_length(x);
    msb = get_msb(x);
    while(GET_CAR(msb) == 0 && dig > 1){
        msb = prev(msb);
        dig--;
    }
    SET_CDR(msb,NIL);
}
int bigx_int_to_big(int x){
    int res,y;
    
    y = GET_INT(x);
    res = gen_big();
    SET_TAG(res,BIGX);
    cons_next(abs(y),res);
    if(y>=0)
        set_sign(res,1);
    else
        set_sign(res,-1);
    return(res);
}

int bigx_long_to_big(int x){
    int res,i2,i1;
    long long int l;
    
    l = GET_LONG(x);
    i2 = llabs(l) % BIGNUM_BASE; 
    i1 = llabs(l) / BIGNUM_BASE;
    res = gen_big();
    SET_TAG(res,BIGX);
    if(l>=0)
        set_sign(res,1);
    else
        set_sign(res,-1);
    cons_next(i1,cons_next(i2,res));
    return(res);
}

int bigx_simplify(int x){ 
    int i1;
    long long int l,l1,l2;
        
    if(bigx_length(x) == 0){
        return(makeint(0));
    }
    else if(bigx_length(x) == 1){
        i1 = GET_CAR(x);
        i1 = i1 * get_sign(x);
        bigx_gbc(x);
        return(makeint(i1));
    }
    else if(bigx_length(x) == 2){
        l2 = GET_CAR(x);
        l1 = GET_CAR(next(x));
        l = (l1 * BIGNUM_BASE + l2) * get_sign(x);
        bigx_gbc(x);
        return(makelong(l));
    }
    else
        return(x);
    
}
//add n-zero-cells to x
int bigx_shift(int x, int n){
    int res,msb;
    
    msb = res = gen_big();
    SET_TAG(res,BIGX);
    set_sign(res,get_sign(x));
    
    while(n > 0){
        msb = cons_next(0,msb);
        n--;
    }
    do{
        msb = cons_next(GET_CAR(x),msb);
        x = next(x);
    }while(!nullp(x));
    return(res);
}


int bigx_abs(int x){
    int res,msb,y;
    
    msb = res = gen_big();
    SET_TAG(res,BIGX);
    set_sign(res,1);
    do{
        y = GET_CAR(x);
        msb = cons_next(y,msb);
        x = next(x);
    }while(!nullp(x));
    
    return(res);
}


int bigx_plus(int arg1, int arg2){
    int res;
    
    res = UNDEF;
    if(bigx_positivep(arg1) && bigx_positivep(arg2)){
        res = bigx_plus1(arg1,arg2);
        set_sign(res,1);
    }
    else if(bigx_negativep(arg1) && bigx_negativep(arg2)){
        res = bigx_plus1(arg1,arg2);
        set_sign(res,-1);
    }
    else if(bigx_positivep(arg1) && bigx_negativep(arg2)){
        if(bigx_abs_smallerp(arg1,arg2)){
            res = bigx_minus1(arg2,arg1);
            set_sign(res,-1);
        }
        else{
            res = bigx_minus1(arg1,arg2);
            set_sign(res,1);
        }
    }
    else if(bigx_negativep(arg1) && bigx_positivep(arg2)){
        if(bigx_abs_smallerp(arg1,arg2)){
            res = bigx_minus1(arg2,arg1);
            set_sign(res,1);
        }
        else{
            res = bigx_minus1(arg1,arg2); 
            set_sign(res,-1); 
        }
    }
    
    if(simp_flag)
        res = bigx_simplify(res);
        
    return(res);
}

int bigx_plus1(int arg1, int arg2){
    int x,y,z,c,q,res,msb;
    
    msb = res = gen_big();
    SET_TAG(res,BIGX);
    set_sign(res,1);
    c = 0;
    do{
        x = GET_CAR(arg1);
        y = GET_CAR(arg2);
        z = x + y + c;
        c = z / BIGNUM_BASE;
        q = z % BIGNUM_BASE;
        msb = cons_next(q,msb);
        arg1 = next(arg1);
        arg2 = next(arg2); 
    }while(!nullp(arg1) && !nullp(arg2));
    if(nullp(arg1))
        bigx_plus2(arg2,c,msb);
    else
        bigx_plus2(arg1,c,msb);
    cut_zero(res);
    return(res);
}

void bigx_plus2(int arg, int c, int msb){
    int q;
    long long int l;
    
    do{
        l = GET_CAR(arg) + c;
        c = l / BIGNUM_BASE;
        q = l % BIGNUM_BASE;
        msb = cons_next(q,msb);
        arg = next(arg);
    }while(!nullp(arg));
    if(c!=0)
        msb = cons_next(c,msb);
}   

int bigx_minus(int arg1, int arg2){
    int res;
    
    res = UNDEF;
    if(bigx_positivep(arg1) && bigx_positivep(arg2)){
        if(bigx_smallerp(arg1,arg2)){
            res = bigx_minus1(arg2,arg1);
            set_sign(res,-1);
        }
        else{
            res = bigx_minus1(arg1,arg2);
            set_sign(res,1);
        }
    }
    else if(bigx_positivep(arg1) && bigx_negativep(arg2)){
        res = bigx_plus1(arg1,arg2);
        set_sign(res,1);
    }
    else if(bigx_negativep(arg1) && bigx_positivep(arg2)){
        res = bigx_plus1(arg1,arg2);
        set_sign(res,-1);
    }
    else if(bigx_negativep(arg1) && bigx_negativep(arg2)){
        if(bigx_abs_smallerp(arg1,arg2)){
            res = bigx_minus1(arg2,arg1);
            set_sign(res,1);
        }
        else{
            res = bigx_minus1(arg1,arg2);
            set_sign(res,-1);
        }
    }
    if(simp_flag)
        res = bigx_simplify(res);
        
    return(res);        
}

// arg1 > arg2
int bigx_minus1(int arg1, int arg2){
    int x,y,z,c,res,msb;
     
    msb = res = gen_big();
    SET_TAG(res,BIGX);
    set_sign(res,1);
    c = 0;
    do{
        x = GET_CAR(arg1);
        y = GET_CAR(arg2);
        if((x + c - y) < 0){
            z = (x + BIGNUM_BASE + c) - y;
            c = -1;
        }
        else{
            z = (x + c) - y;
            c = 0;
        }
        
        msb = cons_next(z,msb);
        arg1 = next(arg1);
        arg2 = next(arg2);
    }while(!nullp(arg2));

    if(!nullp(arg1))
        bigx_minus2(arg1,c,msb);
    
    cut_zero(res);
    return(res);
}

void bigx_minus2(int arg, int c, int msb){
    int q,x;

    while(!nullp(arg)){
        x = GET_CAR(arg);
        if(x + c  < 0){
            q = x + BIGNUM_BASE + c;
            c = -1;
        }
        else{
            q = x + c;
            c = 0;
        }
        msb = cons_next(q,msb);
        arg = next(arg);
    }
}

int bigx_mult(int arg1, int arg2){
    int res;
    
    res = UNDEF;
    if(bigx_positivep(arg1) && bigx_positivep(arg2)){
        res = bigx_mult1(arg1,arg2);
        set_sign(res,1);
    }
    else if(bigx_positivep(arg1) && bigx_negativep(arg2)){
        res = bigx_mult1(arg1,arg2);
        set_sign(res,-1);
    }
    else if(bigx_negativep(arg1) && bigx_positivep(arg2)){
        res = bigx_mult1(arg1,arg2);
        set_sign(res,-1);
    }
    else if(bigx_negativep(arg1) && bigx_negativep(arg2)){
        res = bigx_mult1(arg1,arg2);
        set_sign(res,1);
    } 
    if(simp_flag)
        res = bigx_simplify(res);
        
    return(res);
}

int bigx_mult1(int arg1, int arg2){
    int res,msb,dig1,dig2,n;
    long long int x,y,z,l1,l2,c;
    
    msb = res = gen_n(bigx_length(arg1)+bigx_length(arg2));
    set_sign(res,1);
    dig2 = arg2;
    n = 0; 
    do{
        dig1 = arg1;
        msb = get_nth(res,n);
        do{
            x = (long long int)GET_CAR(dig1);
            y = (long long int)GET_CAR(dig2);
            z = x * y; 
            l2 = z % BIGNUM_BASE;
            l1 = z / BIGNUM_BASE;
            l2 = l2 + (long long int)GET_CAR(msb);
            if(l2 >= BIGNUM_BASE){
                c = l2 / BIGNUM_BASE;
                l2 = l2 % BIGNUM_BASE;
            }
            else{
                c = 0;
            }
            l1 = l1 + c + (long long int)GET_CAR(next(msb));
            SET_CAR(msb,(int)l2);
            SET_CAR(next(msb),(int)l1);
            dig1 = next(dig1);
            msb = next(msb);
        }while(!nullp(dig1)); 
        dig2 = next(dig2);
        n++;
    }while(!nullp(dig2)); 
    cut_zero(res);
    return(res);
}



int bigx_quotient(int arg1, int arg2){
    int res,x,y;
    
    res = UNDEF;
    //if devidend is smaller than divisor,return 0
    if(bigx_abs_smallerp(arg1,arg2))
        return(makeint(0));
        
    if(bigx_positivep(arg1) && bigx_positivep(arg2)){
        res = bigx_quotient1(arg1,arg2);
    }
    else if(bigx_positivep(arg1) && bigx_negativep(arg2)){
        y = bigx_abs(arg2);
        res = bigx_quotient1(arg1,y);
        set_sign(res,-1);
        bigx_gbc(y);
    }
    else if(bigx_negativep(arg1) && bigx_positivep(arg2)){
        x = bigx_abs(arg1); 
        res = bigx_quotient1(x,arg2);
        set_sign(res,-1);
        bigx_gbc(x);
    }
    else if(bigx_negativep(arg1) && bigx_negativep(arg2)){
        x = bigx_abs(arg1);
        y = bigx_abs(arg2);
        res = bigx_quotient1(x,y);
        set_sign(res,1);
        bigx_gbc(x);
        bigx_gbc(y);
    }
    //else
    //  exception("big_quotient", ILLEGAL_ARGUMENT, list2(arg1,arg2));
    
    if(simp_flag)
        res = bigx_simplify(res);
    return(res);
}
/*
Knuth TAOCP fourth. see p88
s=sift is for match digits, ds is divisor, p=plus adding again, 
bv= if u/v > BIGNUM_BASE, cell of MSB.

*/

//#define TEST

int bigx_quotient1(int arg1, int arg2){ 
    int s,ss,ds,p,t,res,flag1,flag2,i,dig1,dig2; 
    long long int d,u,v,q,l1,l2;
    #ifdef TEST
    int c;
    c = 10;
    #endif    

    flag1 = 0;
    flag2 = 0;
    ss = 0;
    //devidend is smaller than 2 times divisor, return 1
    //if quotient is 1, theorem is not hold
    s = bigx_mult1(arg2,bigx_int_to_big(makeint(2)));
    if(bigx_abs_smallerp(arg1,s))
        return(makeint(1));
    
    //following code, calcuration is required in bignum 
    //so, stop simlification.
    simp_flag = 0;
    
    res = gen_big();
    //if divisor is smaller, become bigger to hold theolem 
    v = (long long int)GET_CAR(get_msb(arg2));
    if(v < (BIGNUM_BASE / 2)){
        d = BIGNUM_BASE / (v + 1);
        arg1 = bigx_mult1(arg1,bigx_long_to_big(makelong(d)));
        arg2 = bigx_mult1(arg2,bigx_long_to_big(makelong(d)));
        flag1 = 1;
        flag2 = 1;
    }
    
    #ifdef TEST
    printf("arg1=");print(arg1);printf("\n");
    printf("arg2=");print(arg2);printf("\n");
    #endif
    
    do{ dig1 = bigx_length(arg1);
        dig2 = bigx_length(arg2);
        if(dig1 > dig2){
            l1 = (long long int)GET_CAR(get_msb(arg1));
            l2 = (long long int)GET_CAR(prev(get_msb(arg1)));
            u = l1 * BIGNUM_BASE + l2;
            v = (long long int)GET_CAR(get_msb(arg2));
        }
        else{
            u = (long long int)GET_CAR(get_msb(arg1));
            v = (long long int)GET_CAR(get_msb(arg2));
        }
            
        q = u / v;
        if(q == 1 && dig1!=dig2)
            q = 10;
        ds = bigx_mult1(arg2,bigx_long_to_big(makelong(q)));
        
    
        s = bigx_length(arg1)- bigx_length(ds);
        ds = bigx_shift(ds,s);
        p = bigx_shift(arg2,s); 
        #ifdef TEST
        printf("q= %I64d (u=%I64d/ v=%I64d)\n", q,u,v);
        printf("ds=");print(ds);printf("\n");
        printf("p=");print(p);printf("\n");
        printf("s=%d\n", s);
        #endif
        t = bigx_minus(arg1,ds);
        if(flag1)
            bigx_gbc(arg1); //garbage collection
        arg1 = t;
        flag1 = 1;
        
        
        
        while(bigx_negativep(arg1)){ 
            t = bigx_plus(arg1,p);
            bigx_gbc(arg1); //garbage collection
            arg1 = t;
            q--;
        } 
        #ifdef TEST
        printf("after arg1=");print(arg1);printf("\n");
        #endif

        bigx_gbc(ds);//garbage collection
        bigx_gbc(p);
        
        if((ss - s) > 1){
            i = ss- s;
            while(i>1){
                res = cons_prev(res,0);
                i--;
            }
        }
        //if q is bigger than BIGNUM_BASE, number span two cells. 
        if(q < BIGNUM_BASE){
            res = cons_prev(res,(int)q);
        }
        else{
            l2 = q % BIGNUM_BASE;
            l1 = q / BIGNUM_BASE;
            res = cons_prev(cons_prev(res,(int)l1),(int)l2);
        }
        ss = s;
        #ifdef TEST
        c--;
        if(c==0)
            {DEBUG}
        #endif
    }while(!bigx_abs_smallerp(arg1,arg2));
    
    res = bigx_shift(res,s);
    
    if(flag2)
        bigx_gbc(arg2);
                
    //restore flag
    simp_flag = 1;
    SET_TAG(res,BIGX);
    set_sign(res,1);
    cut_zero(res);
    return(res);
}


int bigx_big_to_flt(int x){
    double val;
    int i,msb,res;
    
    res = freshcell();
    val = 0.0;
    msb = get_msb(x);
    do{
        i = GET_CAR(msb);
        val = val * (double)BIGNUM_BASE + (double)i;
        msb = prev(msb);
    }while(!nullp(msb));
    val = val * get_sign(x);
    SET_TAG(res,FLTN);
    SET_FLT(res,val);
    return(res);
}

//bignum remainder of bignum and int
int bigx_remainder_i(int x, int y){
    int msb,sign1,sign2;
    long long int i,j,r;
    
    
    msb = get_msb(bigx_abs(x));
    sign1 = get_sign(x);
    
    j = GET_INT(y);
    if(j < 0){
        j = abs(j);
        sign2 = -1;
    }
    else{
        sign2 = 1;
    }
        
    r = 0;
    
    do{
        i = GET_CAR(msb);
        i = i + r * BIGNUM_BASE;
        if(i >= j)
            r = i % j;
        else
            r = i + r;
        msb = prev(msb);
    }while(!nullp(msb));
    return(makeint((int)r*sign1*sign2));
}

//quotient of bignum and int
int bigx_quotient_i(int x, int y){
    int res,msb,sign1,sign2;
    long long int i,j,r,q;
    
    
    res = gen_big();
    msb = get_msb(bigx_abs(x));
    sign1 = get_sign(x);
    
    j = GET_INT(y);
    if(j < 0){
        j = abs(j);
        sign2 = -1;
    }
    else{
        sign2 = 1;
    }
        
    r = 0;
    
    do{
        i = GET_CAR(msb);
        i = i + r * BIGNUM_BASE;
        if(i >= j){
            r = i % j;
            q = i / j;
            res = cons_prev(res,(int)q);
        }
        else{
            r = i + r;
            res = cons_prev(res,0);
        }
        msb = prev(msb);
    }while(!nullp(msb));
    SET_TAG(res,BIGX);
    cut_zero(res);
    set_sign(res,sign1*sign2);
    res = bigx_simplify(res);
    return(res);
}

//multiple of bignum and int
int bigx_mult_i(int x, int y){
    int res,msb,sign1,sign2;
    long long int i,j,c,z;
    
    
    msb = res = gen_big();
    sign1 = get_sign(x);
    
    j = GET_INT(y);
    if(j < 0){
        j = abs(j);
        sign2 = -1;
    }
    else{
        sign2 = 1;
    }
        
    c = 0;
    
    do{
        i = GET_CAR(x);
        z = i * j + c;
        if(z >= BIGNUM_BASE){
            c = z / BIGNUM_BASE;
            z = z % BIGNUM_BASE;
            msb = cons_next((int)z,msb);
        }
        else{
            c = 0;
            msb = cons_next((int)z,msb);
        }
        x = next(x);
    }while(!nullp(x));
    if(c != 0)
        msb = cons_next((int)c,msb);
    SET_TAG(res,BIGX);
    cut_zero(res);
    set_sign(res,sign1*sign2);
    res = bigx_simplify(res);
    return(res);
}
