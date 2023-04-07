#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if(x>=im.w)x=im.w-1;
    if(y>=im.h)y=im.h-1;
    if(c>=im.c)c=im.c-1;
    if(x<0)x=0;
    if(y<0)y=0;
    if(c<0)c=0;
    return im.data[c*im.w*im.h + y*im.w + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if(x>=im.w)return;
    if(y>=im.h)return;
    if(c>=im.c)return;
    if(x<0)return;
    if(y<0)return;
    if(c<0)return;
    im.data[c*im.w*im.h + y*im.w + x]=v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data,im.data,im.c*im.w*im.h*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      {
      float a=0;
      a+=get_pixel(im,q1,q2,0)*0.299;
      a+=get_pixel(im,q1,q2,1)*0.587;
      a+=get_pixel(im,q1,q2,2)*0.114;
      set_pixel(gray,q1,q2,0,a);
      }
    return gray;
}

void shift_image(image im, int c, float v)
{
    assert(c>=0 && c<im.c);
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      set_pixel(im,q1,q2,c,get_pixel(im,q1,q2,c)+v);
}

void clamp_image(image im)
{
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      for(int q3=0;q3<im.c;q3++)
        {
        float a=get_pixel(im,q1,q2,q3);
        if(a<0)a=0;
        if(a>1)a=1;
        set_pixel(im,q1,q2,q3,a);
        }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      {
      float r=get_pixel(im,q1,q2,0);
      float g=get_pixel(im,q1,q2,1);
      float b=get_pixel(im,q1,q2,2);
      
      float V=three_way_max(r,g,b);
      float m=three_way_min(r,g,b);
      float c=V-m;
      float S=V==0?0:c/V;
      
      float H=0;
      
      if(c==0)H=0;
      else if(V==r)H=(g-b)/c+0;
      else if(V==g)H=(b-r)/c+2;
      else if(V==b)H=(r-g)/c+4;
      
      if(H<0)H+=6;
      H/=6;
      
      set_pixel(im,q1,q2,0,H);
      set_pixel(im,q1,q2,1,S);
      set_pixel(im,q1,q2,2,V);
      }
}

void hsv_to_rgb(image im)
{
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      {
      float h=get_pixel(im,q1,q2,0);
      float s=get_pixel(im,q1,q2,1);
      float v=get_pixel(im,q1,q2,2);
      
      float c=s*v;
      float m=v-c;
      float r=0,g=0,b=0;
      
      h*=6;
      if(h>5)h-=6;
      
           if(h<=0){h-=0;r=v;g=m;b=m-h*c;} //[5,6] -> [-1,0]

      else if(h<=1){h-=0;r=v;b=m;g=m+h*c;} //[0,1] -> [0,1]

      else if(h<=2){h-=2;g=v;b=m;r=m-h*c;} //[1,2] -> [-1,0]

      else if(h<=3){h-=2;g=v;r=m;b=m+h*c;} //[2,3] -> [0,1]

      else if(h<=4){h-=4;b=v;r=m;g=m-h*c;} //[3,4] -> [-1,0]

      else if(h<=5){h-=4;b=v;g=m;r=m+h*c;} //[4,5] -> [0,1]

      else {printf("error %d %d  %f\n",q1,q2,h);}
      
      set_pixel(im,q1,q2,0,r);
      set_pixel(im,q1,q2,1,g);
      set_pixel(im,q1,q2,2,b);
      
      
      }
}


struct RGBcolor { float r,g,b; };
struct XYZcolor { float x,y,z; };
struct LCHcolor { float l,c,h; };

typedef struct RGBcolor RGBcolor;
typedef struct XYZcolor XYZcolor;
typedef struct LCHcolor LCHcolor;

float l2g(float a)
  {
  if(a<0.0031308)return 12.92*a;
  else return 1.055*powf(a,1.0f/2.4f)-0.055;
  }

float g2l(float a)
  {
  if(a<0.04045)return a/12.92;
  else return powf((a+0.055f)/1.055f,2.4f);
  }

RGBcolor linear2gamma(RGBcolor a)
  {
  a.r=l2g(a.r);
  a.g=l2g(a.g);
  a.b=l2g(a.b);
  return a;
  }

RGBcolor gamma2linear(RGBcolor a)
  {
  a.r=g2l(a.r);
  a.g=g2l(a.g);
  a.b=g2l(a.b);
  return a;
  }

XYZcolor toXYZ(RGBcolor a)
  {
  XYZcolor b;
  a=gamma2linear(a);
  b.x=0.412383*a.r+0.357585*a.g+0.18048  *a.b;
  b.y=0.212635*a.r+0.71517 *a.g+0.072192 *a.b;
  b.z=0.01933 *a.r+0.119195*a.g+0.950528 *a.b;
  return b;
  }


RGBcolor toRGB(XYZcolor a)
  {
  RGBcolor b;
  b.r=(3.24103  )*a.x+(-1.53741 )*a.y +(-0.49862 )*a.z;
  b.g=(-0.969242)*a.x+(1.87596  )*a.y +(0.041555 )*a.z;
  b.b=(0.055632 )*a.x+(-0.203979)*a.y +(1.05698  )*a.z;
  b=linear2gamma(b);
  return b;
  }

LCHcolor rgb2lch(RGBcolor a)
  {
  LCHcolor b={0.f,0.f,0.f};
  XYZcolor c=toXYZ(a);
  
  if(c.x==0.f && c.y==0.f && c.z==0.f)return b;
  
  //printf("xyz1   %f %f %f\n",c.x,c.y,c.z);
  
  float u1=4*c.x/(1*c.x+15*c.y+3*c.z);
  float v1=9*c.y/(1*c.x+15*c.y+3*c.z);
  
  
  float un=0.2009;
  float vn=0.4610;
  
  float cutoff=powf(6.f/29.f,3);
  
  float l=0;
  if(c.y<=cutoff)l=powf(29.f/3.f,3)*c.y;
  else l=116.f*powf(c.y,1.f/3.f)-16.f;
  float u=13.f*l*(u1-un);
  float v=13.f*l*(v1-vn);
  
  
  b.l=l;
  b.c=sqrtf(u*u+v*v);
  b.h=atan2f(u,v);
  
  return b;
  }

RGBcolor lch2rgb(LCHcolor a)
  {
  XYZcolor b={0.f,0.f,0.f};
  
  if(a.l==0.f && a.c==0.f && a.h==0.f)return toRGB(b);
  
  float u=a.c*sinf(a.h);
  float v=a.c*cosf(a.h);
  float l=a.l;
  
  float un=0.2009;
  float vn=0.4610;
  
  float cutoff=8;
  
  
  float u1=u/(13.f*l)+un;
  float v1=v/(13.f*l)+vn;
  
  if(l<=cutoff)b.y=l*powf(3.f/29.f,3);
  else b.y=powf((l+16.f)/116.f,3);
  
  b.x=b.y*(9*u1)/(4*v1);
  b.z=b.y*(12-3*u1-20*v1)/(4*v1);
  
  //printf("xyz2   %f %f %f\n",b.x,b.y,b.z);
  
  return toRGB(b);
  }


void rgb_to_lch(image im)
{
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      {
      RGBcolor a;
      a.r=get_pixel(im,q1,q2,0);
      a.g=get_pixel(im,q1,q2,1);
      a.b=get_pixel(im,q1,q2,2);
      
      LCHcolor b=rgb2lch(a);
      
      if(isnan(b.c)){printf("%f %f %f\n",a.r,a.g,a.b);printf("%f %f %f\n",b.l,b.c,b.h);}
      if(isnan(b.h)){printf("%f %f %f\n",a.r,a.g,a.b);printf("%f %f %f\n",b.l,b.c,b.h);}
      if(isnan(b.l)){printf("%f %f %f\n",a.r,a.g,a.b);printf("%f %f %f\n",b.l,b.c,b.h);}
      
      set_pixel(im,q1,q2,0,b.l);
      set_pixel(im,q1,q2,1,b.c);
      set_pixel(im,q1,q2,2,b.h);
      }
}

void lch_to_rgb(image im)
{   
    
    for(float q1=0.f;q1<=1.f;q1+=0.1f)
    for(float q2=0.f;q2<=1.f;q2+=0.1f)
    for(float q3=0.f;q3<=1.f;q3+=0.1f)
      {
      RGBcolor a={q1,q2,q3};
      LCHcolor b=rgb2lch(a);
      RGBcolor c=lch2rgb(b);
      
      float err=0;
      err+=fabsf(c.r-a.r);
      err+=fabsf(c.g-a.g);
      err+=fabsf(c.b-a.b);
      
      if(err>0.0001f)
        {
        printf("%f %f %f\n",a.r,a.g,a.b);
        printf("%f %f %f\n",b.l,b.c,b.h);
        printf("%f %f %f\n",c.r,c.g,c.b);
        }
      }
    
    
    //exit(0);
    
    for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
      {
      LCHcolor a;
      a.l=get_pixel(im,q1,q2,0);
      a.c=get_pixel(im,q1,q2,1);
      a.h=get_pixel(im,q1,q2,2);
      
      RGBcolor b=lch2rgb(a);
      
      set_pixel(im,q1,q2,0,b.r);
      set_pixel(im,q1,q2,1,b.g);
      set_pixel(im,q1,q2,2,b.b);
      
      }
}