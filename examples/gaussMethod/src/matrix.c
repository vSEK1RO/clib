#include "matrix.h"

#define flagcheck(flag) if(flag!=CLIB_SUCCESS)return flag
clib_flag mtrx_init(mtrx * out, uint64_t m, uint64_t n, uint64_t size, mtrx_field * field)
{
    clib_flag flag=CLIB_UNNAMED;
    if(size!=field->size)return CLIB_TYPE_INCORRECT;
    flag=clib_arr_eInit((clib_arr*)out,m,sizeof(void*),sizeof(void*));
    flagcheck(flag);
    for(uint64_t i=0;i<m;i++){
        flag=clib_arr_init((clib_arr*)clib_arr_get((clib_arr*)out,i),n,size);
        flagcheck(flag);
    }
    flag=clib_mem_copy((void*)clib_arr_eGet((clib_arr*)out,0),(void*)&field,sizeof(void*));
    return flag;
}
uint64_t mtrx_height(mtrx * m)
{
    return clib_arr_len((clib_arr*)m);
}
uint64_t mtrx_width(mtrx * m)
{
    return clib_arr_len((clib_arr*)clib_arr_get((clib_arr*)m,0));
}
uint64_t mtrx_size(mtrx * m)
{
    return clib_arr_size((clib_arr*)clib_arr_get((clib_arr*)m,0));
}
mtrx_item mtrx_get(mtrx * m, uint64_t im, uint64_t jn)
{
    mtrx_item item=NULL;
    if(im>mtrx_height(m) || jn>mtrx_width(m))return item;
    item=(mtrx_item)clib_arr_get((clib_arr*)clib_arr_get((clib_arr*)m,im),jn);
    return item;
}
mtrx_field * mtrx_fGet(mtrx * m)
{
    return *(mtrx_field**)clib_arr_eGet((clib_arr*)m,0);
}
clib_flag mtrx_del(mtrx * m)
{
    clib_flag flag=CLIB_UNNAMED;
    for(uint64_t i=0;i<mtrx_height(m);i++){
        clib_arr * buff=(clib_arr*)clib_arr_get((clib_arr*)m,i);
        flag=clib_arr_del(buff);
        flagcheck(flag);
    }
    flag=clib_arr_del((clib_arr*)m);
    return flag;
}
clib_flag mtrx_subRow(mtrx * m, uint64_t i1, uint64_t i2)
{
    clib_flag flag=CLIB_UNNAMED;
    clib_arr * a=(clib_arr*)clib_arr_get((clib_arr*)m,i1);
    clib_arr * b=(clib_arr*)clib_arr_get((clib_arr*)m,i2);
    for(uint64_t i=0;i<clib_arr_len(a);i++){
        flag=mtrx_fGet(m)->sub(clib_arr_get(a,i),clib_arr_get(a,i),clib_arr_get(b,i));
        flagcheck(flag);
    }
    return flag;
}
clib_flag mtrx_mutRow(mtrx * m, uint64_t im, mtrx_item lambda)
{
    clib_flag flag=CLIB_UNNAMED;
    clib_arr * a=(clib_arr*)clib_arr_get((clib_arr*)m,im);
    for(uint64_t i=0;i<clib_arr_len(a);i++){
        flag=mtrx_fGet(m)->mut(clib_arr_get(a,i),clib_arr_get(a,i),lambda);
        flagcheck(flag);
    }
    return flag;
}
clib_flag mtrx_scan(mtrx * m)
{
    if(mtrx_size(m)!=mtrx_fGet(m)->size)return CLIB_TYPE_INCORRECT;
    clib_flag flag=CLIB_UNNAMED;
    for(uint64_t i=0;i<mtrx_height(m);i++){
        for(uint64_t j=0;j<mtrx_width(m);j++){
            flag=mtrx_fGet(m)->scan(mtrx_get(m,i,j));
            flagcheck(flag);
        }
    }
    return CLIB_SUCCESS;
}
clib_flag mtrx_print(mtrx * m, char * sep, char * end)
{
    if(mtrx_size(m)!=mtrx_fGet(m)->size)return CLIB_TYPE_INCORRECT;
    clib_flag flag=CLIB_UNNAMED;
    for(uint64_t i=0;i<mtrx_height(m);i++){
        for(uint64_t j=0;j<mtrx_width(m);j++){
            flag=mtrx_fGet(m)->print(mtrx_get(m,i,j));
            flagcheck(flag);
            flag=mtrx_fGet(m)->prints(sep);
            flagcheck(flag);
        }
        flag=mtrx_fGet(m)->prints(end);
        flagcheck(flag);
    }
    return CLIB_SUCCESS;
}
clib_flag mtrx_concat(mtrx * out, mtrx * m, mtrx * b)
{
    clib_flag flag=CLIB_UNNAMED;
    if(mtrx_height(m)!=mtrx_height(b))return CLIB_TYPE_INCORRECT;
    flag=mtrx_init(
        out,
        mtrx_height(m),
        mtrx_width(m)+mtrx_width(b),
        mtrx_size(m),
        mtrx_fGet(m)
    );
    flagcheck(flag);
    clib_arr buff;
    flag=clib_arr_init(&buff,mtrx_width(out),mtrx_size(m));
    for(uint64_t i=0;i<mtrx_height(m);i++){
        flagcheck(flag);
        flag=clib_arr_concat(
            &buff,
            clib_arr_get((clib_arr*)m,i),
            clib_arr_get((clib_arr*)b,i)
        );
        flagcheck(flag);
        flag=clib_mem_copy(
            *(clib_arr*)clib_arr_get((clib_arr*)out,i),
            buff,
            clib_arr_size(&buff)*clib_arr_len(&buff)  
        );
        flagcheck(flag);
        flag=clib_arr_del(&buff);
    }
    return flag;
}
clib_flag mtrx_splitByColumn(mtrx * out1, mtrx * out2, mtrx * m, uint64_t j)
{
    clib_flag flag=CLIB_UNNAMED;
    flag=mtrx_init(out1,mtrx_height(m),j,mtrx_size(m),mtrx_fGet(m));
    flagcheck(flag);
    flag=mtrx_init(out2,mtrx_height(m),mtrx_width(m)-j,mtrx_size(m),mtrx_fGet(m));
    clib_arr buff;
    for(uint64_t i=0;i<mtrx_height(out1);i++){
        flagcheck(flag);
        flag=clib_arr_substr(
            &buff,
            clib_arr_get((clib_arr*)m,i),
            0,
            j
        );
        flagcheck(flag);
        flag=clib_mem_copy(
            *(clib_arr*)clib_arr_get((clib_arr*)out1,i),
            buff,
            clib_arr_size(&buff)*clib_arr_len(&buff)  
        );
        flagcheck(flag);
        flag=clib_arr_del(&buff);
        flagcheck(flag);
        flag=clib_arr_substr(
            &buff,
            clib_arr_get((clib_arr*)m,i),
            j,
            mtrx_width(m)
        );
        flagcheck(flag);
        flag=clib_mem_copy(
            *(clib_arr*)clib_arr_get((clib_arr*)out2,i),
            buff,
            clib_arr_size(&buff)*clib_arr_len(&buff)  
        );
        flagcheck(flag);
        flag=clib_arr_del(&buff);
    }
    return flag;
}