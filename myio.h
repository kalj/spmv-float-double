#ifndef _MYIO_H
#define _MYIO_H

#include <cstdio>

int sscanval(const char *str, int &dst)
{
    return sscanf(str,"%d",&dst);
}

int sscanval(const char *str, float &dst)
{
    return sscanf(str,"%f",&dst);
}

int sscanval(const char *str, double &dst)
{
    return sscanf(str,"%lf",&dst);
}

#endif /* _MYIO_H */
